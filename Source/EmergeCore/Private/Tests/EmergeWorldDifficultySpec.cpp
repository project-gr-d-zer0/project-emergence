// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: depth-based difficulty progression.
// Core of the canonical design: difficulty scales with DEPTH (how far into the connected maps you go), not time.
// CONTRACT to implement — NEW class UEmergeWorldDifficulty : public UBlueprintFunctionLibrary
//   in Public/World/EmergeWorldDifficulty.h + Private/World/EmergeWorldDifficulty.cpp, static UFUNCTIONs:
//
//   static int32 HordeDensityAtDepth(int32 Depth);
//     // horde gets denser the deeper you go. First-pass (tunable) formula:
//     // D = FMath::Max(0, Depth); return 10 + D * 15;
//
//   static int32 EnemyGearTierAtDepth(int32 Depth);
//     // enemy gear steps up with depth, capped. Formula: return FMath::Clamp(Depth, 0, 4);
//     // tier 0 = basic raiders (bolt-action/pistol); tier >=1 = factions (plate armor, automatics).
// Deterministic, exact golden values, boundaries covered. Numbers are a first-pass balance pass; tune later.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "World/EmergeWorldDifficulty.h"

BEGIN_DEFINE_SPEC(FEmergeWorldDifficultySpec, "Emergence.World.Difficulty",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeWorldDifficultySpec)
void FEmergeWorldDifficultySpec::Define()
{
	Describe("HordeDensityAtDepth (denser the deeper you go: 10 + max(0,Depth)*15)", [this]()
	{
		It("matches exact golden values and is monotonic non-decreasing", [this]()
		{
			TestEqual(TEXT("depth 0 -> 10"),   UEmergeWorldDifficulty::HordeDensityAtDepth(0), 10);
			TestEqual(TEXT("depth 1 -> 25"),   UEmergeWorldDifficulty::HordeDensityAtDepth(1), 25);
			TestEqual(TEXT("depth 2 -> 40"),   UEmergeWorldDifficulty::HordeDensityAtDepth(2), 40);
			TestEqual(TEXT("depth 5 -> 85"),   UEmergeWorldDifficulty::HordeDensityAtDepth(5), 85);
			TestEqual(TEXT("negative depth clamps to depth 0 -> 10"), UEmergeWorldDifficulty::HordeDensityAtDepth(-3), 10);
			for (int32 D = 0; D < 20; ++D)
			{
				TestTrue(TEXT("non-decreasing in depth"),
					UEmergeWorldDifficulty::HordeDensityAtDepth(D + 1) >= UEmergeWorldDifficulty::HordeDensityAtDepth(D));
			}
		});
	});

	Describe("EnemyGearTierAtDepth (steps up with depth, capped at 4)", [this]()
	{
		It("matches exact golden values and boundaries", [this]()
		{
			TestEqual(TEXT("depth 0 -> tier 0"),          UEmergeWorldDifficulty::EnemyGearTierAtDepth(0), 0);
			TestEqual(TEXT("depth 1 -> tier 1"),          UEmergeWorldDifficulty::EnemyGearTierAtDepth(1), 1);
			TestEqual(TEXT("depth 4 -> tier 4 (cap)"),    UEmergeWorldDifficulty::EnemyGearTierAtDepth(4), 4);
			TestEqual(TEXT("depth 9 -> tier 4 (clamped)"),UEmergeWorldDifficulty::EnemyGearTierAtDepth(9), 4);
			TestEqual(TEXT("negative -> tier 0 (clamped)"),UEmergeWorldDifficulty::EnemyGearTierAtDepth(-2), 0);
		});
	});
}
#endif
