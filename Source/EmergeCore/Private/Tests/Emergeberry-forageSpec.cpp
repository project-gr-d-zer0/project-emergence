// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: berry/plant foraging yield (survival).
// Foraging a bush/patch yields food by how rich the area is, with a tool bonus; the patch then regrows over time
// (richer patches regrow faster). Feeds EmergeDigestion. TUNABLE design values, fixed deterministic model.
// CONTRACT — NEW class UEmergeForage : public UBlueprintFunctionLibrary
//   in Public/Survival/EmergeForage.h + Private/Survival/EmergeForage.cpp, static UFUNCTIONs.
//   Richness int32: 0 Barren, 1 Sparse, 2 Lush.
//
//   static int32 YieldForArea(int32 Richness, bool bTool);
//     // int32 y; switch(Richness){ case 1: y=1; break; case 2: y=3; break; default: y=0; }
//     // if (bTool && y > 0) y += 1;   return y;
//   static float RegrowTimeSec(int32 Richness);
//     // switch(Richness){ case 1: return 600.0f; case 2: return 300.0f; default: return -1.0f; }  // barren never regrows
//   static bool  IsForageable(int32 Richness);   // return Richness >= 1;
//
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Survival/EmergeForage.h"

BEGIN_DEFINE_SPEC(FEmergeForageSpec, "Emergence.Survival.Forage",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeForageSpec)
void FEmergeForageSpec::Define()
{
	Describe("YieldForArea", [this]()
	{
		It("richer areas yield more; tool adds one", [this]()
		{
			TestEqual(TEXT("barren -> 0"), UEmergeForage::YieldForArea(0, false), 0);
			TestEqual(TEXT("sparse -> 1"), UEmergeForage::YieldForArea(1, false), 1);
			TestEqual(TEXT("lush -> 3"), UEmergeForage::YieldForArea(2, false), 3);
			TestEqual(TEXT("lush + tool -> 4"), UEmergeForage::YieldForArea(2, true), 4);
			TestEqual(TEXT("barren + tool still 0"), UEmergeForage::YieldForArea(0, true), 0);
		});
	});

	Describe("Regrow & forageable", [this]()
	{
		It("regrow time and forageability", [this]()
		{
			TestEqual(TEXT("sparse regrows 600s"), UEmergeForage::RegrowTimeSec(1), 600.0f);
			TestEqual(TEXT("lush regrows 300s"), UEmergeForage::RegrowTimeSec(2), 300.0f);
			TestEqual(TEXT("barren never (-1)"), UEmergeForage::RegrowTimeSec(0), -1.0f);
			TestFalse(TEXT("barren not forageable"), UEmergeForage::IsForageable(0));
			TestTrue(TEXT("sparse forageable"), UEmergeForage::IsForageable(1));
		});
	});
}
#endif
