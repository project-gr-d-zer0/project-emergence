#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeMeleeGate.generated.h"

// Zombie melee attack gates (research: Kung-Fu Circle) — range band + facing dot + attack-capacity
// budget, plus hitstop timing. Deterministic, data-agnostic math only.
UCLASS()
class EMERGECORE_API UEmergeMeleeGate : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	// Inclusive range band check: MinUu <= DistUu && DistUu <= MaxUu.
	UFUNCTION(BlueprintCallable, Category = "Combat|Melee")
	static bool InAttackRange(float DistUu, float MinUu, float MaxUu);

	// Facing gate: DotToTarget >= MinDot (inclusive).
	UFUNCTION(BlueprintCallable, Category = "Combat|Melee")
	static bool PassesFacingGate(float DotToTarget, float MinDot);

	// Attack-capacity budget: (ActiveWeight + RequestWeight) <= Capacity (inclusive).
	UFUNCTION(BlueprintCallable, Category = "Combat|Melee")
	static bool BudgetAllows(int32 ActiveWeight, int32 RequestWeight, int32 Capacity);

	// Hitstop duration in seconds at 60fps: heavy -> 10 frames, light -> 4 frames.
	UFUNCTION(BlueprintCallable, Category = "Combat|Melee")
	static float HitstopSeconds(bool bHeavy);
};
