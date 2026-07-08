#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EmergeStaminaComponent.generated.h"

// Runtime stamina: sprinting drains stamina (heavier encumbrance load drains faster), and it regenerates
// while not sprinting. Connects the inventory encumbrance load-tier to movement. Simulate() is pure state
// math (headless-testable); gameplay code feeds it the sprint state + current load tier each frame.
UCLASS(ClassGroup = (Emerge), meta = (BlueprintSpawnableComponent))
class EMERGECORE_API UEmergeStaminaComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	UEmergeStaminaComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emerge|Stamina")
	float Stamina = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emerge|Stamina")
	float SprintDrainPerSec = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emerge|Stamina")
	float RegenPerSec = 5.0f;

	// Advances stamina. Sprinting drains SprintDrainPerSec * (1 + LoadTier*0.5) per second;
	// otherwise regenerates RegenPerSec per second. Clamped to [0, 100].
	UFUNCTION(BlueprintCallable, Category = "Emerge|Stamina")
	void Simulate(float DeltaSeconds, bool bSprinting, int32 LoadTier);

	UFUNCTION(BlueprintPure, Category = "Emerge|Stamina")
	bool CanSprint() const;   // Stamina > 0

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
