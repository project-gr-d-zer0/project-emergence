// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: suppression flinch (combat/gunplay).
// Near-miss rounds suppress: the closer they pass, the stronger the aim disturbance, which then decays over time
// once fire stops. TUNABLE design model, fixed deterministic.
// CONTRACT — NEW class UEmergeSuppression : public UBlueprintFunctionLibrary
//   in Public/Combat/EmergeSuppression.h + Private/Combat/EmergeSuppression.cpp, static UFUNCTIONs.
//
//   static float SuppressionLevel(float MissDistance, float MaxSuppressRange);
//     // closer miss -> more suppression: return FMath::Clamp(1.0f - MissDistance / MaxSuppressRange, 0.0f, 1.0f);
//   static float AimPenalty(float SuppressionLevel, float MaxPenalty);   // return SuppressionLevel * MaxPenalty;
//   static float DecayPerTick(float Current, float DecayRate, float DeltaT);
//     // return FMath::Max(0.0f, Current - DecayRate * DeltaT);
//
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Combat/EmergeSuppression.h"

BEGIN_DEFINE_SPEC(FEmergeSuppressionSpec, "Emergence.Combat.Suppression",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeSuppressionSpec)
void FEmergeSuppressionSpec::Define()
{
	Describe("SuppressionLevel & AimPenalty", [this]()
	{
		It("closer misses suppress harder", [this]()
		{
			TestEqual(TEXT("point-blank miss -> full"), UEmergeSuppression::SuppressionLevel(0.f, 10.f), 1.0f);
			TestEqual(TEXT("at range -> 0"), UEmergeSuppression::SuppressionLevel(10.f, 10.f), 0.0f);
			TestEqual(TEXT("half -> 0.5"), UEmergeSuppression::SuppressionLevel(5.f, 10.f), 0.5f);
			TestEqual(TEXT("beyond range -> 0"), UEmergeSuppression::SuppressionLevel(15.f, 10.f), 0.0f);
			TestEqual(TEXT("penalty scales"), UEmergeSuppression::AimPenalty(0.5f, 2.0f), 1.0f);
		});
	});

	Describe("DecayPerTick", [this]()
	{
		It("suppression bleeds off, not below 0", [this]()
		{
			TestEqual(TEXT("1.0 by 0.5 -> 0.5"), UEmergeSuppression::DecayPerTick(1.0f, 0.5f, 1.0f), 0.5f);
			TestEqual(TEXT("small decays to 0"), UEmergeSuppression::DecayPerTick(0.3f, 0.5f, 1.0f), 0.0f);
			TestEqual(TEXT("dt=2 decays double"), UEmergeSuppression::DecayPerTick(1.0f, 0.3f, 2.0f), 0.4f);
		});
	});
}
#endif
