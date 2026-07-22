// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: bleeding sources (DayZ-style).
// Open wounds drain blood each tick at -20/s per source, scaled by the wound's flow intensity (low/med/high)
// AND by "low pressure" - as blood drops toward fatal, bloodloss tapers (a dying body bleeds slower). Feeds the
// already-built EmergeBloodRegen / EmergeShock (blood level -> shock ramp). Numbers exact from decompiled DayZ
// playerconstants.c + bleedingsource.c/bleedingsourcesmanagerserver.c: BLOODLOSS_PER_SEC -20; FLOW low .1 / med
// .6 / high 1; blood_scale = Clamp(InverseLerp(FATAL 2500, LOW_PRESSURE 4000, blood), MIN_MOD .3, 1); source
// duration 300s; burn-source flow modifier .65.
// CONTRACT — NEW class UEmergeBleeding : public UBlueprintFunctionLibrary
//   in Public/Survival/EmergeBleeding.h + Private/Survival/EmergeBleeding.cpp, static UFUNCTIONs.
//   Intensity is an int32: 0 = Low, 1 = Medium, 2 = High.
//
//   static float FlowModifier(int32 Intensity);
//     // switch(Intensity){ case 0: return 0.1f; case 1: return 0.6f; case 2: return 1.0f; default: return 0.f; }
//
//   static float BloodPressureScale(float Blood);
//     // A dying body bleeds slower. return FMath::Clamp(FMath::GetMappedRangeValueUnclamped(...)) -- specifically:
//     // const float s = FMath::Clamp((Blood - 2500.0f) / (4000.0f - 2500.0f), 0.0f, 1.0f); // InverseLerp(FATAL,LOWP,blood)
//     // return FMath::Clamp(s, 0.3f, 1.0f);   // BLEEDING_LOW_PRESSURE_MIN_MOD floor
//
//   static float SourceBloodLossPerTick(int32 Intensity, float Blood, float DeltaT);
//     // return -20.0f * BloodPressureScale(Blood) * DeltaT * FlowModifier(Intensity);  // BLOODLOSS_PER_SEC
//
//   static float TotalBloodLossPerTick(int32 NumSources, int32 Intensity, float Blood, float DeltaT);
//     // return NumSources * SourceBloodLossPerTick(Intensity, Blood, DeltaT);
//
//   static int32 SourceDurationSec();   // return 300;   // BLEEDING_SOURCE_DURATION_NORMAL
//   static float BurnFlowModifier();    // return 0.65f; // BLEEDING_SOURCE_BURN_MODIFIER
//
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Survival/EmergeBleeding.h"

BEGIN_DEFINE_SPEC(FEmergeBleedingSpec, "Emergence.Survival.Bleeding",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeBleedingSpec)
void FEmergeBleedingSpec::Define()
{
	Describe("FlowModifier", [this]()
	{
		It("low/medium/high intensity modifiers (exact DayZ values)", [this]()
		{
			TestEqual(TEXT("low 0.1"), UEmergeBleeding::FlowModifier(0), 0.1f);
			TestEqual(TEXT("medium 0.6"), UEmergeBleeding::FlowModifier(1), 0.6f);
			TestEqual(TEXT("high 1.0"), UEmergeBleeding::FlowModifier(2), 1.0f);
			TestEqual(TEXT("unknown 0"), UEmergeBleeding::FlowModifier(9), 0.0f);
		});
	});

	Describe("BloodPressureScale", [this]()
	{
		It("tapers from 1.0 at/above 4000 down to floor 0.3 at/below fatal 2500", [this]()
		{
			TestEqual(TEXT("full pressure at 4000"), UEmergeBleeding::BloodPressureScale(4000.f), 1.0f);
			TestEqual(TEXT("above 4000 clamps to 1"), UEmergeBleeding::BloodPressureScale(5000.f), 1.0f);
			TestEqual(TEXT("midpoint 3250 -> 0.5"), UEmergeBleeding::BloodPressureScale(3250.f), 0.5f);
			TestEqual(TEXT("3000 -> 1/3"), UEmergeBleeding::BloodPressureScale(3000.f), 1.0f / 3.0f);
			TestEqual(TEXT("fatal 2500 -> floor 0.3"), UEmergeBleeding::BloodPressureScale(2500.f), 0.3f);
			TestEqual(TEXT("below fatal clamps to 0.3"), UEmergeBleeding::BloodPressureScale(1000.f), 0.3f);
		});
	});

	Describe("SourceBloodLossPerTick", [this]()
	{
		It("-20/s * pressure scale * dt * flow", [this]()
		{
			TestEqual(TEXT("high, full blood, dt=1 -> -20"), UEmergeBleeding::SourceBloodLossPerTick(2, 4000.f, 1.0f), -20.0f);
			TestEqual(TEXT("medium, full blood, dt=1 -> -12"), UEmergeBleeding::SourceBloodLossPerTick(1, 4000.f, 1.0f), -12.0f);
			TestEqual(TEXT("low, full blood, dt=1 -> -2"), UEmergeBleeding::SourceBloodLossPerTick(0, 4000.f, 1.0f), -2.0f);
			TestEqual(TEXT("high, 3250 (scale .5), dt=1 -> -10"), UEmergeBleeding::SourceBloodLossPerTick(2, 3250.f, 1.0f), -10.0f);
			TestEqual(TEXT("high, fatal (scale .3), dt=1 -> -6"), UEmergeBleeding::SourceBloodLossPerTick(2, 2500.f, 1.0f), -6.0f);
			TestEqual(TEXT("high, full, dt=2 -> -40"), UEmergeBleeding::SourceBloodLossPerTick(2, 4000.f, 2.0f), -40.0f);
		});
	});

	Describe("TotalBloodLossPerTick & constants", [this]()
	{
		It("sums independent sources; duration + burn modifier exact", [this]()
		{
			TestEqual(TEXT("3 high sources, full, dt=1 -> -60"), UEmergeBleeding::TotalBloodLossPerTick(3, 2, 4000.f, 1.0f), -60.0f);
			TestEqual(TEXT("0 sources -> 0"), UEmergeBleeding::TotalBloodLossPerTick(0, 2, 4000.f, 1.0f), 0.0f);
			TestEqual(TEXT("2 medium, full, dt=1 -> -24"), UEmergeBleeding::TotalBloodLossPerTick(2, 1, 4000.f, 1.0f), -24.0f);
			TestEqual(TEXT("source duration 300s"), UEmergeBleeding::SourceDurationSec(), 300);
			TestEqual(TEXT("burn flow modifier 0.65"), UEmergeBleeding::BurnFlowModifier(), 0.65f);
		});
	});
}
#endif
