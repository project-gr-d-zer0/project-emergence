#pragma once
#include "CoreMinimal.h"
#include "AlsCharacterExample.h"
#include "EmergeCharacter.generated.h"

EMERGECORE_API DECLARE_LOG_CATEGORY_EXTERN(LogEmergeTelemetry, Log, All);

class UEmergeVitalsComponent;
class UEmergeStaggerComponent;
class UEmergeStaminaComponent;
class UEmergeDamageComponent;
class UEmergeStatusEffectComponent;
class UEmergeEquipmentComponent;
class UEmergeInventoryComponent;
class USpringArmComponent;
class UCameraComponent;

// Survivor pawn built on ALS's example character: inherits its complete, proven Enhanced Input
// (move/look/sprint/crouch/jump/aim/mantle/ragdoll) + ALS camera. Adds the full survival runtime suite
// and a minimal stagger->ragdoll bridge (knockdown = ragdoll; bleeding drains vitals).
UCLASS()
class EMERGECORE_API AEmergeCharacter : public AAlsCharacterExample
{
	GENERATED_BODY()
public:
	AEmergeCharacter();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Emerge") TObjectPtr<UEmergeVitalsComponent> Vitals;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Emerge") TObjectPtr<UEmergeStaggerComponent> Stagger;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Emerge") TObjectPtr<UEmergeStaminaComponent> Stamina;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Emerge") TObjectPtr<UEmergeDamageComponent> Damage;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Emerge") TObjectPtr<UEmergeStatusEffectComponent> StatusEffects;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Emerge") TObjectPtr<UEmergeEquipmentComponent> Equipment;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Emerge") TObjectPtr<UEmergeInventoryComponent> Inventory;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Emerge|Camera") TObjectPtr<USpringArmComponent> CameraBoom;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Emerge|Camera") TObjectPtr<UCameraComponent> FollowCamera;

	virtual void Tick(float DeltaSeconds) override;
	virtual void CalcCamera(float DeltaTime, struct FMinimalViewInfo& OutResult) override;

	// Smart pathing: Claude picks a target, the engine paths + this follows it (no AIController).
	UFUNCTION(BlueprintCallable, Category = "Emerge|Nav")
	bool NavigateTo(FVector Destination);
	UFUNCTION(BlueprintCallable, Category = "Emerge|Nav")
	void StopNavigating();
	UFUNCTION(BlueprintCallable, Category = "Emerge|Nav")
	FString GetNavProgress();

	// Structured spatial snapshot for autonomous testing: player movement, game camera POV,
	// LIDAR rays (walls/floor/ceiling distances + hit names), and nearby actors. Read via Remote Control.
	UFUNCTION(BlueprintCallable, Category = "Emerge|Sensor")
	FString SenseEnvironment(float Radius = 4000.0f);

private:
	// Persistent world-memory occupancy grid (2m cells): key=packed(cx,cy), val 1=free 2=obstacle.
	TMap<int64, uint8> OccGrid;
	float StuckTime = 0.0f;   // seconds moving-input held but not moving (mismatch/glitch oracle)
	float AirTime = 0.0f;     // seconds since last grounded
	TArray<FVector> NavPath;
	int32 NavIdx = 0;
	bool bNavigating = false;
	FVector NavGoal = FVector::ZeroVector;
	float NavStuckTime = 0.0f;
	FString NavState = TEXT("idle");
};
