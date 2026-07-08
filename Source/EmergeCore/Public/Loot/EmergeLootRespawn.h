#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeLootRespawn.generated.h"

// Persistent-world loot respawn math (DayZ Central Economy model).
// Per item type: nominal (world target), min (restock trigger), lifetime (despawn), restock (delay).
UCLASS()
class EMERGECORE_API UEmergeLootRespawn : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	// Restock only triggers at/below Min; returns how many more to spawn toward Nominal.
	UFUNCTION(BlueprintCallable, Category = "Loot|Respawn")
	static int32 SpawnDeficit(int32 Nominal, int32 Current, int32 MinCount);

	// True once an item's age reaches its lifetime. A LifetimeSeconds of 0 means permanent (never despawns).
	UFUNCTION(BlueprintCallable, Category = "Loot|Respawn")
	static bool ShouldDespawn(float AgeSeconds, float LifetimeSeconds);

	// True once enough time has passed since the last restock. Negative delays are treated as 0.
	UFUNCTION(BlueprintCallable, Category = "Loot|Respawn")
	static bool RestockReady(float SinceLastRestockSeconds, float RestockDelaySeconds);
};
