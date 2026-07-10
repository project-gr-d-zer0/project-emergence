// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: zombie melee attack gates
// (research: Kung-Fu Circle — range band + facing dot + attack-capacity budget; hitstop frames).
// CONTRACT — NEW class UEmergeMeleeGate : public UBlueprintFunctionLibrary
//   in Public/Combat/EmergeMeleeGate.h + Private/Combat/EmergeMeleeGate.cpp, static UFUNCTIONs:
//   static bool InAttackRange(float DistUu, float MinUu, float MaxUu);   // Min <= Dist && Dist <= Max
//   static bool PassesFacingGate(float DotToTarget, float MinDot);       // DotToTarget >= MinDot
//   static bool BudgetAllows(int32 ActiveWeight, int32 RequestWeight, int32 Capacity);
//     // = (ActiveWeight + RequestWeight) <= Capacity
//   static float HitstopSeconds(bool bHeavy);   // heavy -> 10.f/60.f, light -> 4.f/60.f
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Combat/EmergeMeleeGate.h"

BEGIN_DEFINE_SPEC(FEmergeMeleeGateSpec, "Emergence.Combat.MeleeGate",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeMeleeGateSpec)
void FEmergeMeleeGateSpec::Define()
{
	Describe("range and facing gates", [this]()
	{
		It("range band is inclusive on both ends", [this]()
		{
			TestTrue(TEXT("120 in [80,180]"), UEmergeMeleeGate::InAttackRange(120.f, 80.f, 180.f));
			TestTrue(TEXT("80 boundary"), UEmergeMeleeGate::InAttackRange(80.f, 80.f, 180.f));
			TestTrue(TEXT("180 boundary"), UEmergeMeleeGate::InAttackRange(180.f, 80.f, 180.f));
			TestFalse(TEXT("79 out"), UEmergeMeleeGate::InAttackRange(79.f, 80.f, 180.f));
			TestFalse(TEXT("181 out"), UEmergeMeleeGate::InAttackRange(181.f, 80.f, 180.f));
		});
		It("facing gate compares dot inclusively", [this]()
		{
			TestTrue(TEXT("0.6 vs 0.5"), UEmergeMeleeGate::PassesFacingGate(0.6f, 0.5f));
			TestTrue(TEXT("0.5 boundary"), UEmergeMeleeGate::PassesFacingGate(0.5f, 0.5f));
			TestFalse(TEXT("0.4 fails"), UEmergeMeleeGate::PassesFacingGate(0.4f, 0.5f));
		});
	});
	Describe("attack budget + hitstop", [this]()
	{
		It("budget allows up to capacity inclusive", [this]()
		{
			TestTrue(TEXT("3+4 <= 8"), UEmergeMeleeGate::BudgetAllows(3, 4, 8));
			TestTrue(TEXT("4+4 <= 8 boundary"), UEmergeMeleeGate::BudgetAllows(4, 4, 8));
			TestFalse(TEXT("5+4 > 8"), UEmergeMeleeGate::BudgetAllows(5, 4, 8));
		});
		It("hitstop is 4 frames light, 10 heavy at 60fps", [this]()
		{
			TestNearlyEqual(TEXT("light"), UEmergeMeleeGate::HitstopSeconds(false), 4.f/60.f, 1e-5f);
			TestNearlyEqual(TEXT("heavy"), UEmergeMeleeGate::HitstopSeconds(true), 10.f/60.f, 1e-5f);
		});
	});
}
#endif
