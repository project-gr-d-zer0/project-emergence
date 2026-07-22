// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: use-based skill leveling.
// XP accumulates; levels unlock at cumulative banked thresholds (Project Zomboid style). Cap level 5.
// CONTRACT — NEW class UEmergeSkill : public UBlueprintFunctionLibrary
//   in Public/Progression/EmergeSkill.h + Private/Progression/EmergeSkill.cpp, static UFUNCTIONs:
//   static int32 LevelForXP(int32 XP);
//     // cumulative thresholds: L1=75, L2=225, L3=525, L4=1275, L5=2775. Return highest level whose
//     // threshold <= XP, capped at 5; below 75 -> 0.
//   static int32 XPToNextLevel(int32 XP);
//     // XP needed to reach the next level: nextThreshold - XP; at level 5 (XP>=2775) return 0.
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Progression/EmergeSkill.h"

BEGIN_DEFINE_SPEC(FEmergeSkillSpec, "Emergence.Progression.Skill",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeSkillSpec)
void FEmergeSkillSpec::Define()
{
	Describe("LevelForXP (banked thresholds, cap 5)", [this]()
	{
		It("matches exact golden values and boundaries", [this]()
		{
			TestEqual(TEXT("0 -> 0"), UEmergeSkill::LevelForXP(0), 0);
			TestEqual(TEXT("74 -> 0"), UEmergeSkill::LevelForXP(74), 0);
			TestEqual(TEXT("75 -> 1"), UEmergeSkill::LevelForXP(75), 1);
			TestEqual(TEXT("224 -> 1"), UEmergeSkill::LevelForXP(224), 1);
			TestEqual(TEXT("225 -> 2"), UEmergeSkill::LevelForXP(225), 2);
			TestEqual(TEXT("525 -> 3"), UEmergeSkill::LevelForXP(525), 3);
			TestEqual(TEXT("3000 -> 5 (cap)"), UEmergeSkill::LevelForXP(3000), 5);
		});
	});
	Describe("XPToNextLevel", [this]()
	{
		It("matches exact golden values", [this]()
		{
			TestEqual(TEXT("at 0 -> 75"), UEmergeSkill::XPToNextLevel(0), 75);
			TestEqual(TEXT("at 75 -> 150"), UEmergeSkill::XPToNextLevel(75), 150);
			TestEqual(TEXT("at cap -> 0"), UEmergeSkill::XPToNextLevel(2775), 0);
		});
	});
}
#endif
