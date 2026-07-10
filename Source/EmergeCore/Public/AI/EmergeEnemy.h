#pragma once
#include "CoreMinimal.h"
#include "AlsCharacter.h"
#include "EmergeEnemy.generated.h"

class UAlsMovementSettings;
class UAlsMantlingSettings;
enum class EAlsMantlingType : uint8;

// Phase A enemy: a full ALS character — the SAME locomotion + mantle system as the player, so
// its runs are speed-matched (no foot-sliding fixed loops) and its wall traversal is the
// IDENTICAL mantle the player performs (clip-mimicry on the mannequin rig kept reading wrong:
// vault=flip, mantle-clip=sandbox dependency flood, hurdle=still not a mantle — all measured).
// Zombie speeds live in its own movement settings: walk=shamble 150, run=chase 560.
UCLASS()
class EMERGECORE_API AEmergeEnemy : public AAlsCharacter
{
	GENERATED_BODY()
public:
	AEmergeEnemy();

	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual UAlsMantlingSettings* SelectMantlingSettings_Implementation(EAlsMantlingType MantlingType) override;

	// Anim-state oracle for RC (debugging pose/anim issues).
	UFUNCTION(BlueprintCallable, Category = "Emerge|Anim")
	FString GetAnimDebug() const;

protected:
	UPROPERTY() TObjectPtr<UAlsMovementSettings> BaseMovementSettings;   // MS_Als_Normal (curves source)
	UPROPERTY() TObjectPtr<UAlsMantlingSettings> MantlingSettingsHigh;
	UPROPERTY() TObjectPtr<UAlsMantlingSettings> MantlingSettingsLow;
};
