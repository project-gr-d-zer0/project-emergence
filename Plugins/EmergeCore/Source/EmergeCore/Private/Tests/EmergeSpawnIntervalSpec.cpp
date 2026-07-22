// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: horde spawn cadence by depth.
// Connects depth-difficulty <-> spawning: hordes respawn faster the deeper you go.
// CONTRACT — ADD ONE static UFUNCTION to the EXISTING class UEmergeWorldDifficulty (do NOT rename/duplicate;
//   keep HordeDensityAtDepth + EnemyGearTierAtDepth exactly), in the existing
//   Public/World/EmergeWorldDifficulty.h + Private/World/EmergeWorldDifficulty.cpp:
//   static float HordeSpawnIntervalSeconds(int32 Depth);
//     // return FMath::Max(2.0f, 20.0f - FMath::Max(0, Depth) * 2.0f);   // floor 2s
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "World/EmergeWorldDifficulty.h"

BEGIN_DEFINE_SPEC(FEmergeSpawnIntervalSpec, "Emergence.World.SpawnInterval",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeSpawnIntervalSpec)
void FEmergeSpawnIntervalSpec::Define()
{
	Describe("HordeSpawnIntervalSeconds (faster deeper)", [this]()
	{
		It("steps down with depth and floors at 2s", [this]()
		{
			TestEqual(TEXT("depth 0 -> 20s"),  UEmergeWorldDifficulty::HordeSpawnIntervalSeconds(0), 20.f);
			TestEqual(TEXT("depth 5 -> 10s"),  UEmergeWorldDifficulty::HordeSpawnIntervalSeconds(5), 10.f);
			TestEqual(TEXT("depth 9 -> 2s"),   UEmergeWorldDifficulty::HordeSpawnIntervalSeconds(9), 2.f);
			TestEqual(TEXT("depth 20 -> 2s (floor)"), UEmergeWorldDifficulty::HordeSpawnIntervalSeconds(20), 2.f);
			TestEqual(TEXT("negative depth -> 20s"), UEmergeWorldDifficulty::HordeSpawnIntervalSeconds(-3), 20.f);
		});
	});
}
#endif
