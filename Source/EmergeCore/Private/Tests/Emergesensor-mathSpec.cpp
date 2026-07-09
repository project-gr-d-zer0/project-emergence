// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: the deterministic decision math the
// SenseEnvironment telemetry relies on - extracted into pure, golden-tested functions so the sensor's
// affordance/consequence/normalization/occupancy-grid logic is UNIT-VERIFIED (fixes ad-hoc thresholds).
// CONTRACT — NEW class UEmergeSensorMath : public UBlueprintFunctionLibrary
//   in Public/Sensor/EmergeSensorMath.h + Private/Sensor/EmergeSensorMath.cpp, static UFUNCTIONs:
//
//   static bool CanMoveForward(float WallAheadD, float FloorDropAhead, bool bLedge);
//     // Clear to walk forward: no near wall, not a ledge, drop not too big.
//     // return (WallAheadD < 0.0f || WallAheadD >= 100.0f) && !bLedge && FloorDropAhead <= 100.0f;
//
//   static int32 ConsequenceCode(float WallAheadD, float FloorDropAhead, bool bLedge);
//     // 1 = blocked (wall within 100), else 2 = dropoff (ledge or drop > 200), else 0 = clear.
//     // if (WallAheadD >= 0.0f && WallAheadD < 100.0f) return 1;
//     // if (bLedge || FloorDropAhead > 200.0f) return 2;
//     // return 0;
//
//   static float NormalizeClamped(float Value, float MaxValue);
//     // return (MaxValue <= 0.0f) ? 0.0f : FMath::Clamp(Value / MaxValue, 0.0f, 1.0f);
//
//   static int64 PackCell(int32 CellX, int32 CellY);
//     // return ((int64)CellX << 32) | (int64)((uint32)CellY);
//   static int32 UnpackCellX(int64 Key);   // return (int32)(Key >> 32);
//   static int32 UnpackCellY(int64 Key);   // return (int32)(Key & 0xFFFFFFFF);
//
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Sensor/EmergeSensorMath.h"

BEGIN_DEFINE_SPEC(FEmergeSensorMathSpec, "Emergence.Sensor.Math",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeSensorMathSpec)
void FEmergeSensorMathSpec::Define()
{
	Describe("CanMoveForward", [this]()
	{
		It("clear only with no near wall, no ledge, small drop", [this]()
		{
			TestTrue(TEXT("no wall, drop 50 -> true"), UEmergeSensorMath::CanMoveForward(-1.f, 50.f, false));
			TestFalse(TEXT("wall at 80 -> false"), UEmergeSensorMath::CanMoveForward(80.f, 50.f, false));
			TestFalse(TEXT("ledge -> false"), UEmergeSensorMath::CanMoveForward(-1.f, 50.f, true));
			TestFalse(TEXT("big drop 150 -> false"), UEmergeSensorMath::CanMoveForward(-1.f, 150.f, false));
			TestTrue(TEXT("wall far at 100 -> true"), UEmergeSensorMath::CanMoveForward(100.f, 50.f, false));
		});
	});
	Describe("ConsequenceCode (0 clear/1 blocked/2 dropoff)", [this]()
	{
		It("matches exact golden values", [this]()
		{
			TestEqual(TEXT("wall 80 -> blocked 1"), UEmergeSensorMath::ConsequenceCode(80.f, 50.f, false), 1);
			TestEqual(TEXT("drop 250 -> dropoff 2"), UEmergeSensorMath::ConsequenceCode(-1.f, 250.f, false), 2);
			TestEqual(TEXT("ledge -> dropoff 2"), UEmergeSensorMath::ConsequenceCode(-1.f, 50.f, true), 2);
			TestEqual(TEXT("open -> clear 0"), UEmergeSensorMath::ConsequenceCode(-1.f, 50.f, false), 0);
		});
	});
	Describe("NormalizeClamped", [this]()
	{
		It("matches exact golden values and guards", [this]()
		{
			TestEqual(TEXT("50/100 -> 0.5"), UEmergeSensorMath::NormalizeClamped(50.f, 100.f), 0.5f);
			TestEqual(TEXT("150/100 clamps -> 1"), UEmergeSensorMath::NormalizeClamped(150.f, 100.f), 1.f);
			TestEqual(TEXT("zero max -> 0"), UEmergeSensorMath::NormalizeClamped(5.f, 0.f), 0.f);
		});
	});
	Describe("Cell key pack/unpack round-trips (incl. negatives)", [this]()
	{
		It("recovers signed cell coords", [this]()
		{
			TestEqual(TEXT("x of (3,-2)"), UEmergeSensorMath::UnpackCellX(UEmergeSensorMath::PackCell(3, -2)), 3);
			TestEqual(TEXT("y of (3,-2)"), UEmergeSensorMath::UnpackCellY(UEmergeSensorMath::PackCell(3, -2)), -2);
			TestEqual(TEXT("x of (-40,17)"), UEmergeSensorMath::UnpackCellX(UEmergeSensorMath::PackCell(-40, 17)), -40);
			TestEqual(TEXT("y of (-40,17)"), UEmergeSensorMath::UnpackCellY(UEmergeSensorMath::PackCell(-40, 17)), 17);
		});
	});
}
#endif
