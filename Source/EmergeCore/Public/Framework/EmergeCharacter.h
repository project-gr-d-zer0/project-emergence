#pragma once
#include "CoreMinimal.h"
#include "AlsCharacterExample.h"
#include "EmergeCharacter.generated.h"

EMERGECORE_API DECLARE_LOG_CATEGORY_EXTERN(LogEmergeTelemetry, Log, All);

enum class EAlsMantlingType : uint8;
class UAlsMantlingSettings;

class UEmergeVitalsComponent;
class UEmergeStaggerComponent;
class UEmergeStaminaComponent;
class UEmergeDamageComponent;
class UEmergeStatusEffectComponent;
class UEmergeEquipmentComponent;
class UEmergeInventoryComponent;
class USpringArmComponent;
class UCameraComponent;
class UNavigationInvokerComponent;

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
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Emerge|Nav") TObjectPtr<UNavigationInvokerComponent> NavInvoker;

	virtual void Tick(float DeltaSeconds) override;
	virtual void CalcCamera(float DeltaTime, struct FMinimalViewInfo& OutResult) override;

	// ALS asks the character which mantling settings to use; the example BP assigns these in-editor,
	// our C++ reparent must provide them or StartMantling ensures (caught live 2026-07-09).
	virtual UAlsMantlingSettings* SelectMantlingSettings_Implementation(EAlsMantlingType MantlingType) override;

protected:
	UPROPERTY() TObjectPtr<UAlsMantlingSettings> MantlingSettingsHigh;
	UPROPERTY() TObjectPtr<UAlsMantlingSettings> MantlingSettingsLow;

public:

	// Smart pathing: Claude picks a target, the engine paths + this follows it (no AIController).
	UFUNCTION(BlueprintCallable, Category = "Emerge|Nav")
	bool NavigateTo(FVector Destination);
	UFUNCTION(BlueprintCallable, Category = "Emerge|Nav")
	void StopNavigating();
	UFUNCTION(BlueprintCallable, Category = "Emerge|Nav")
	FString GetNavProgress();

	// Programmatic sprint (RC/AI callable; the ALS input path needs a held key we can't hold remotely).
	// Denied while sprint-exhausted (stamina economy).
	UFUNCTION(BlueprintCallable, Category = "Emerge|Nav")
	void SetSprinting(bool bSprint);

	// Stamina below which sprint stays locked after exhaustion (hysteresis; re-arms at this value).
	UPROPERTY(EditAnywhere, Category = "Emerge|Stamina") float SprintReArmStamina = 20.0f;

	// Dynamic escape: sample a fan of flee candidates away from the threat (flee kernel direction),
	// score each by real path reachability + escape distance gained - detour penalty, sprint to the best.
	UFUNCTION(BlueprintCallable, Category = "Emerge|Nav")
	bool FleeFrom(FVector ThreatPos);

	// Structured spatial snapshot for autonomous testing: player movement, game camera POV,
	// LIDAR rays (walls/floor/ceiling distances + hit names), and nearby actors. Read via Remote Control.
	UFUNCTION(BlueprintCallable, Category = "Emerge|Sensor")
	FString SenseEnvironment(float Radius = 4000.0f);

	// World-level telemetry (AI census, navmesh, influence belief, NPC anomalies) forwarded from
	// UEmergeWorldSense — kept on the character because this is the proven RC object path.
	UFUNCTION(BlueprintCallable, Category = "Emerge|Sensor")
	FString SenseWorld();

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
	FGameplayTag PrevRotationMode;
	bool bNavRotationOverridden = false;
	bool bFleeing = false;   // FleeFrom engaged sprint; RestoreNavFacing releases it
	bool bSprintExhausted = false;   // stamina hit 0; locked until SprintReArmStamina
	float NavTurnErrorDeg = 0.0f;
	int32 NavRepathCount = 0;
	int32 NavVaultCount = 0;   // auto-vaults triggered while path-following (stuck -> ALS mantle)
	float NavLastDist = -1.0f;
	bool bNavMakingProgress = false;
	bool ComputePathTo(FVector Destination);
	void RestoreNavFacing();
};
