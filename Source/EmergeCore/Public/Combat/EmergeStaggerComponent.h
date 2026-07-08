#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Combat/EmergeStagger.h"
#include "EmergeStaggerComponent.generated.h"

// Runtime stagger component: accumulates a stagger meter from incoming hits (via UEmergeStagger),
// recovers it over time, and exposes the current locomotion state + speed multiplier a character's
// movement should read. Tuning is an editor-editable struct (defaults = tested golden values).
UCLASS(ClassGroup = (Emerge), meta = (BlueprintSpawnableComponent))
class EMERGECORE_API UEmergeStaggerComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	UEmergeStaggerComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emerge|Stagger")
	FEmergeStaggerTuning Tuning;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emerge|Stagger")
	float Meter = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emerge|Stagger")
	float RecoveryPerSec = 20.0f;

	// Adds stagger from a hit (body-part + direction weighted) to the meter.
	UFUNCTION(BlueprintCallable, Category = "Emerge|Stagger")
	void ApplyHit(float Damage, EEmergeBodyPart Part, EEmergeHitDir Dir);

	// Recovers the meter toward 0 by DeltaSeconds.
	UFUNCTION(BlueprintCallable, Category = "Emerge|Stagger")
	void Recover(float DeltaSeconds);

	UFUNCTION(BlueprintPure, Category = "Emerge|Stagger")
	EEmergeStaggerState CurrentState() const;

	// Locomotion speed multiplier [0..1] the owner's movement should apply this frame.
	UFUNCTION(BlueprintPure, Category = "Emerge|Stagger")
	float SpeedMultiplier() const;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
