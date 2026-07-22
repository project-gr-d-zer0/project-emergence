// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: boil-purify water (survival).
// Boiling water over heat purifies it: progress accumulates while on heat, and once the required boil time is
// reached the contamination chance from EmergeWaterPurity drops to zero. Composes with EmergeCampfire /
// EmergeWaterPurity. TUNABLE boil time, fixed deterministic model.
// CONTRACT — NEW class UEmergeWaterBoil : public UBlueprintFunctionLibrary
//   in Public/Survival/EmergeWaterBoil.h + Private/Survival/EmergeWaterBoil.cpp, static UFUNCTIONs.
//
//   static float BoilProgressPerTick(bool bOnHeat, float DeltaT);   // return bOnHeat ? DeltaT : 0.0f;
//   static bool  IsPurified(float BoilProgress, float BoilTimeReq);  // return BoilProgress >= BoilTimeReq;
//   static float ContaminationAfterBoil(float BaseChance, bool bPurified);   // return bPurified ? 0.0f : BaseChance;
//
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Survival/EmergeWaterBoil.h"

BEGIN_DEFINE_SPEC(FEmergeWaterBoilSpec, "Emergence.Survival.WaterBoil",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeWaterBoilSpec)
void FEmergeWaterBoilSpec::Define()
{
	Describe("Boil & purify", [this]()
	{
		It("accumulates on heat and removes contamination once boiled", [this]()
		{
			TestEqual(TEXT("on heat dt=1 -> 1"), UEmergeWaterBoil::BoilProgressPerTick(true, 1.0f), 1.0f);
			TestEqual(TEXT("off heat -> 0"), UEmergeWaterBoil::BoilProgressPerTick(false, 1.0f), 0.0f);
			TestTrue(TEXT("60/60 purified"), UEmergeWaterBoil::IsPurified(60.f, 60.f));
			TestFalse(TEXT("59/60 not yet"), UEmergeWaterBoil::IsPurified(59.f, 60.f));
			TestEqual(TEXT("purified -> 0 contamination"), UEmergeWaterBoil::ContaminationAfterBoil(0.3f, true), 0.0f);
			TestEqual(TEXT("unboiled -> base contamination"), UEmergeWaterBoil::ContaminationAfterBoil(0.3f, false), 0.3f);
		});
	});
}
#endif
