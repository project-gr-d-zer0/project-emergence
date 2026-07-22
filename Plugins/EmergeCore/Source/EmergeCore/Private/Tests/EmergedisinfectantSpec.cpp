// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: disinfectant (survival medical).
// Disinfectant treats an EARLY wound infection (EmergeWoundInfection stage 1) by cutting wound agents; a severe
// (stage 2) infection is past it and needs antibiotics. TUNABLE reduction rate, fixed deterministic model.
// CONTRACT — NEW class UEmergeDisinfectant : public UBlueprintFunctionLibrary
//   in Public/Survival/EmergeDisinfectant.h + Private/Survival/EmergeDisinfectant.cpp, static UFUNCTIONs.
//
//   static bool  CanCure(int32 InfectionStage);          // return InfectionStage <= 1;  // prevents/cures early only
//   static bool  RequiresAntibiotics(int32 InfectionStage);   // return InfectionStage >= 2;
//   static float AgentReductionPerTick(float DeltaT);     // return 10.0f * DeltaT;
//
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Survival/EmergeDisinfectant.h"

BEGIN_DEFINE_SPEC(FEmergeDisinfectantSpec, "Emergence.Survival.Disinfectant",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeDisinfectantSpec)
void FEmergeDisinfectantSpec::Define()
{
	Describe("Cure eligibility", [this]()
	{
		It("cures early infection, severe needs antibiotics", [this]()
		{
			TestTrue(TEXT("stage 0 (clean) treatable"), UEmergeDisinfectant::CanCure(0));
			TestTrue(TEXT("stage 1 curable"), UEmergeDisinfectant::CanCure(1));
			TestFalse(TEXT("stage 2 not curable by disinfectant"), UEmergeDisinfectant::CanCure(2));
			TestTrue(TEXT("stage 2 requires antibiotics"), UEmergeDisinfectant::RequiresAntibiotics(2));
			TestFalse(TEXT("stage 1 no antibiotics needed"), UEmergeDisinfectant::RequiresAntibiotics(1));
		});
	});

	Describe("AgentReductionPerTick", [this]()
	{
		It("reduces wound agents while applied", [this]()
		{
			TestEqual(TEXT("dt=1 -> 10"), UEmergeDisinfectant::AgentReductionPerTick(1.0f), 10.0f);
			TestEqual(TEXT("dt=2 -> 20"), UEmergeDisinfectant::AgentReductionPerTick(2.0f), 20.0f);
		});
	});
}
#endif
