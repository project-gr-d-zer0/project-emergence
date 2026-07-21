// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: shelter build progress (survival).
// Building a shelter needs enough materials, then accrues build progress over time until complete; a finished
// shelter is a heat/rest site (EmergeHeatSource / EmergeCampsite). TUNABLE build speed, fixed deterministic model.
// CONTRACT — NEW class UEmergeShelter : public UBlueprintFunctionLibrary
//   in Public/Survival/EmergeShelter.h + Private/Survival/EmergeShelter.cpp, static UFUNCTIONs.
//
//   static bool  CanBuild(int32 MaterialsHave, int32 MaterialsReq);   // return MaterialsHave >= MaterialsReq;
//   static float BuildProgressPerTick(bool bBuilding, float BuildSpeed, float DeltaT);
//     // return bBuilding ? (BuildSpeed * DeltaT) : 0.0f;
//   static bool  IsComplete(float Progress, float Required);   // return Progress >= Required;
//
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Survival/EmergeShelter.h"

BEGIN_DEFINE_SPEC(FEmergeShelterSpec, "Emergence.Survival.Shelter",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeShelterSpec)
void FEmergeShelterSpec::Define()
{
	Describe("Shelter build", [this]()
	{
		It("needs materials, accrues progress, completes", [this]()
		{
			TestTrue(TEXT("enough materials"), UEmergeShelter::CanBuild(5, 3));
			TestFalse(TEXT("not enough materials"), UEmergeShelter::CanBuild(2, 3));
			TestEqual(TEXT("building dt=1 @2 -> 2"), UEmergeShelter::BuildProgressPerTick(true, 2.f, 1.f), 2.0f);
			TestEqual(TEXT("not building -> 0"), UEmergeShelter::BuildProgressPerTick(false, 2.f, 1.f), 0.0f);
			TestTrue(TEXT("100/100 complete"), UEmergeShelter::IsComplete(100.f, 100.f));
			TestFalse(TEXT("99/100 not complete"), UEmergeShelter::IsComplete(99.f, 100.f));
		});
	});
}
#endif
