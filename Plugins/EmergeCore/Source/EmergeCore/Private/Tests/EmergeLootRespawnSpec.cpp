// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: persistent-world loot respawn math
// (DayZ Central Economy model). Per item type: nominal (world target), min (restock trigger),
// lifetime (despawn), restock (delay). Without this a persistent no-raid-timer world strip-mines empty.
// CONTRACT — NEW class UEmergeLootRespawn : public UBlueprintFunctionLibrary
//   in Public/Loot/EmergeLootRespawn.h + Private/Loot/EmergeLootRespawn.cpp, static UFUNCTIONs:
//   static int32 SpawnDeficit(int32 Nominal, int32 Current, int32 MinCount);
//     // if (Current > MinCount) return 0;              // restock only triggers at/below min
//     // return FMath::Max(0, Nominal - Current);
//   static bool ShouldDespawn(float AgeSeconds, float LifetimeSeconds);
//     // return LifetimeSeconds > 0.0f && AgeSeconds >= LifetimeSeconds;
//   static bool RestockReady(float SinceLastRestockSeconds, float RestockDelaySeconds);
//     // return SinceLastRestockSeconds >= FMath::Max(0.0f, RestockDelaySeconds);
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Loot/EmergeLootRespawn.h"

BEGIN_DEFINE_SPEC(FEmergeLootRespawnSpec, "Emergence.Loot.Respawn",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeLootRespawnSpec)
void FEmergeLootRespawnSpec::Define()
{
	Describe("SpawnDeficit (refill toward nominal at/below min)", [this]()
	{
		It("matches exact golden values and boundaries", [this]()
		{
			TestEqual(TEXT("20 nominal, 5 current, min 8 -> 15"), UEmergeLootRespawn::SpawnDeficit(20, 5, 8), 15);
			TestEqual(TEXT("at min triggers -> 12"), UEmergeLootRespawn::SpawnDeficit(20, 8, 8), 12);
			TestEqual(TEXT("above min -> 0"), UEmergeLootRespawn::SpawnDeficit(20, 9, 8), 0);
			TestEqual(TEXT("over-nominal floors at 0"), UEmergeLootRespawn::SpawnDeficit(20, 25, 30), 0);
		});
	});

	Describe("ShouldDespawn (lifetime cleanup)", [this]()
	{
		It("expires at lifetime; zero lifetime never expires", [this]()
		{
			TestTrue(TEXT("age 100 >= lifetime 100"), UEmergeLootRespawn::ShouldDespawn(100.f, 100.f));
			TestFalse(TEXT("age 99 < lifetime 100"), UEmergeLootRespawn::ShouldDespawn(99.f, 100.f));
			TestFalse(TEXT("lifetime 0 = permanent"), UEmergeLootRespawn::ShouldDespawn(9999.f, 0.f));
		});
	});

	Describe("RestockReady (delay honored)", [this]()
	{
		It("ready at/after the delay", [this]()
		{
			TestTrue(TEXT("60 >= 60"), UEmergeLootRespawn::RestockReady(60.f, 60.f));
			TestFalse(TEXT("59 < 60"), UEmergeLootRespawn::RestockReady(59.f, 60.f));
			TestTrue(TEXT("negative delay treated as 0"), UEmergeLootRespawn::RestockReady(0.f, -5.f));
		});
	});
}
#endif
