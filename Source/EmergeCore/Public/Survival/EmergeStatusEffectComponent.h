#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EmergeStatusEffectComponent.generated.h"

class UEmergeVitalsComponent;

// Runtime status effects (first: bleeding). A penetrating wound starts a bleed that drains health over
// time via the tested UEmergeMedical kernel; a bandage/medkit stops it. Simulate() is pure state math
// applied to a passed-in vitals component (headless-testable).
UCLASS(ClassGroup = (Emerge), meta = (BlueprintSpawnableComponent))
class EMERGECORE_API UEmergeStatusEffectComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	UEmergeStatusEffectComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emerge|Status")
	bool bBleeding = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emerge|Status")
	float BleedRatePerSec = 2.0f;

	// Begin bleeding at the given rate (e.g. from a penetrating hit).
	UFUNCTION(BlueprintCallable, Category = "Emerge|Status")
	void StartBleeding(float RatePerSec);

	// Apply a medical item (bandage = 1, medkit = 2). Returns true if it stopped the bleeding.
	UFUNCTION(BlueprintCallable, Category = "Emerge|Status")
	bool ApplyMedical(int32 ItemType);

	// Advance effects by DeltaSeconds, draining the target vitals health. Returns damage dealt this step.
	UFUNCTION(BlueprintCallable, Category = "Emerge|Status")
	float Simulate(float DeltaSeconds, UEmergeVitalsComponent* Vitals);
};
