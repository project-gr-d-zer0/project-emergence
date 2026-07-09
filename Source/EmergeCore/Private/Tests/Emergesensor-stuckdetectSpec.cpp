// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: NPC stuck-detection oracle math.
// Mirrors the player's pressedNotMoving glitch oracle for AI pawns: accumulates time an agent is
// expected to be moving but isn't; resets the moment it moves or stops trying. Feeds world telemetry.
// CONTRACT — CREATE ONE NEW class UEmergeStuckDetect (UBlueprintFunctionLibrary, EMERGECORE_API) in
//   NEW files Public/Sensor/EmergeStuckDetect.h + Private/Sensor/EmergeStuckDetect.cpp:
//   static float Accumulate(float Current, float Speed, float MinSpeed, bool bExpectedMoving, float DtSeconds);
//     // if (!bExpectedMoving || Speed >= MinSpeed) { return 0.0f; }
//     // return Current + FMath::Max(0.0f, DtSeconds);
//   static bool IsStuck(float StuckSeconds, float ThresholdSeconds);
//     // return StuckSeconds >= FMath::Max(ThresholdSeconds, 0.1f);
// Deterministic, exact golden values, boundaries covered. Every function body wrapped in braces;
// every code path must return a value.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Sensor/EmergeStuckDetect.h"

BEGIN_DEFINE_SPEC(FEmergeStuckDetectSpec, "Emergence.Sensor.StuckDetect",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeStuckDetectSpec)
void FEmergeStuckDetectSpec::Define()
{
	Describe("Accumulate (stuck time integration)", [this]()
	{
		It("accumulates while trying to move but slow", [this]()
		{
			TestEqual(TEXT("0 + 0.5s slow -> 0.5"), UEmergeStuckDetect::Accumulate(0.f, 5.f, 10.f, true, 0.5f), 0.5f);
			TestEqual(TEXT("1 + 0.5s slow -> 1.5"), UEmergeStuckDetect::Accumulate(1.f, 5.f, 10.f, true, 0.5f), 1.5f);
			TestEqual(TEXT("zero speed counts"), UEmergeStuckDetect::Accumulate(0.25f, 0.f, 10.f, true, 0.25f), 0.5f);
		});
		It("resets when moving or not expected to move", [this]()
		{
			TestEqual(TEXT("at threshold speed -> 0"), UEmergeStuckDetect::Accumulate(2.f, 10.f, 10.f, true, 0.5f), 0.f);
			TestEqual(TEXT("fast -> 0"), UEmergeStuckDetect::Accumulate(2.f, 300.f, 10.f, true, 0.5f), 0.f);
			TestEqual(TEXT("idle by choice -> 0"), UEmergeStuckDetect::Accumulate(2.f, 0.f, 10.f, false, 0.5f), 0.f);
		});
		It("ignores negative dt", [this]()
		{
			TestEqual(TEXT("negative dt adds nothing"), UEmergeStuckDetect::Accumulate(2.f, 5.f, 10.f, true, -1.f), 2.f);
		});
	});
	Describe("IsStuck (threshold with 0.1s floor)", [this]()
	{
		It("matches exact golden values", [this]()
		{
			TestEqual(TEXT("3 >= 3 -> true"), UEmergeStuckDetect::IsStuck(3.f, 3.f), true);
			TestEqual(TEXT("2.9 < 3 -> false"), UEmergeStuckDetect::IsStuck(2.9f, 3.f), false);
			TestEqual(TEXT("threshold floors at 0.1: 0.05 -> false"), UEmergeStuckDetect::IsStuck(0.05f, 0.f), false);
			TestEqual(TEXT("threshold floors at 0.1: 0.2 -> true"), UEmergeStuckDetect::IsStuck(0.2f, 0.f), true);
		});
	});
}
#endif

