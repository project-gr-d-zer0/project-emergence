#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EmergeVitalsComponent.generated.h"

// Runtime survival component: holds live vitals state and advances it each frame by calling the
// deterministic UEmergeVitals kernel. All logic funnels through Simulate() so it is headless-testable
// (no world/tick required); TickComponent just forwards the frame delta.
UCLASS(ClassGroup = (Emerge), meta = (BlueprintSpawnableComponent))
class EMERGECORE_API UEmergeVitalsComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	UEmergeVitalsComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emerge|Vitals")
	float Health = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emerge|Vitals")
	float Hydration = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emerge|Vitals")
	float HydrationDrainPerSec = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emerge|Vitals")
	float StarveDamagePerSec = 1.0f;

	// Advances Hydration (and Health, if starving) by DeltaSeconds via the tested kernel. Pure state math.
	UFUNCTION(BlueprintCallable, Category = "Emerge|Vitals")
	void Simulate(float DeltaSeconds);

	UFUNCTION(BlueprintPure, Category = "Emerge|Vitals")
	bool IsStarving() const;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
