#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeCrafting.generated.h"

// Deterministic hideout crafting math: can-craft check, craft time by complexity,
// and skill reducing craft time (the progression/"knowledge" differentiator).
UCLASS()
class EMERGECORE_API UEmergeCrafting : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintPure, Category = "Crafting")
	static bool CanCraft(int32 HaveQty, int32 NeedQty);

	UFUNCTION(BlueprintPure, Category = "Crafting")
	static float CraftTimeSeconds(int32 Complexity);

	UFUNCTION(BlueprintPure, Category = "Crafting")
	static float CraftTimeWithSkill(int32 Complexity, int32 SkillLevel);
};
