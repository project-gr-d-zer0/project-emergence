#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EmergeSurvivalNeedsComponent.generated.h"

UCLASS(ClassGroup=(Emerge), meta=(BlueprintSpawnableComponent))
class EMERGECORE_API UEmergeSurvivalNeedsComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Emerge|Needs") float Energy = 5000.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Emerge|Needs") float Water = 5000.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Emerge|Needs") float Blood = 5000.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Emerge|Needs") int32 Movement = 0;         // 0 Idle..3 Sprint
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Emerge|Needs") int32 BleedingSources = 0;

	UFUNCTION(BlueprintCallable, Category="Emerge|Needs")
	void Simulate(float DeltaSeconds);

	UFUNCTION(BlueprintPure, Category="Emerge|Needs")
	bool IsAlive() const;

	UFUNCTION(BlueprintPure, Category="Emerge|Needs")
	int32 ImmunityLevel() const;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};