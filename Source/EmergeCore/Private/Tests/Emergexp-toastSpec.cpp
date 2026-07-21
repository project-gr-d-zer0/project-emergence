// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: XP toast / level-up view-model (shell).
// SHELL stratum: the skill-XP readout - current level, progress toward the next, and whether a gain crossed a
// level boundary (to fire the "level up!" toast). Fixed XP-per-level model; pairs with EmergeSkillXpCurve. Pure
// view math; deterministic.
// CONTRACT — NEW class UEmergeXpToast : public UBlueprintFunctionLibrary
//   in Public/Shell/EmergeXpToast.h + Private/Shell/EmergeXpToast.cpp, static UFUNCTIONs.
//
//   static int32 LevelForXp(int32 Xp, int32 XpPerLevel);       // return Xp / XpPerLevel;
//   static float LevelProgress01(int32 Xp, int32 XpPerLevel);   // return (float)(Xp % XpPerLevel) / (float)XpPerLevel;
//   static bool  DidLevelUp(int32 XpBefore, int32 XpAfter, int32 XpPerLevel);
//     // return LevelForXp(XpAfter, XpPerLevel) > LevelForXp(XpBefore, XpPerLevel);
//
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Shell/EmergeXpToast.h"

BEGIN_DEFINE_SPEC(FEmergeXpToastSpec, "Emergence.Shell.XpToast",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeXpToastSpec)
void FEmergeXpToastSpec::Define()
{
	Describe("LevelForXp & progress", [this]()
	{
		It("integer level and fractional progress", [this]()
		{
			TestEqual(TEXT("250xp @100/lvl -> level 2"), UEmergeXpToast::LevelForXp(250, 100), 2);
			TestEqual(TEXT("0xp -> level 0"), UEmergeXpToast::LevelForXp(0, 100), 0);
			TestEqual(TEXT("250xp progress 0.5"), UEmergeXpToast::LevelProgress01(250, 100), 0.5f);
			TestEqual(TEXT("300xp progress 0.0"), UEmergeXpToast::LevelProgress01(300, 100), 0.0f);
		});
	});

	Describe("DidLevelUp", [this]()
	{
		It("true only when a gain crosses a level boundary", [this]()
		{
			TestTrue(TEXT("90 -> 110 crosses"), UEmergeXpToast::DidLevelUp(90, 110, 100));
			TestFalse(TEXT("110 -> 150 same level"), UEmergeXpToast::DidLevelUp(110, 150, 100));
			TestTrue(TEXT("190 -> 210 crosses to 2"), UEmergeXpToast::DidLevelUp(190, 210, 100));
			TestFalse(TEXT("no gain"), UEmergeXpToast::DidLevelUp(150, 150, 100));
		});
	});
}
#endif
