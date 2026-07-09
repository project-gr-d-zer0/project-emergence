#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EmergeEnemy.generated.h"

class UAnimSequenceBase;

// Phase A enemy: a plain ACharacter (has CharacterMovement so AIController MoveTo works). Slow by default
// (shamble); the controller raises speed when chasing. Locomotion visuals = speed-switched looping
// AnimSequences played single-node (idle/walk/jog). ABP_Unarmed is a linked anim LAYER (frozen pose when
// used standalone) and blendspace single-node playback also failed to evaluate — sequences are the
// simplest path that provably animates.
UCLASS()
class EMERGECORE_API AEmergeEnemy : public ACharacter
{
	GENERATED_BODY()
public:
	AEmergeEnemy();

	virtual void Tick(float DeltaSeconds) override;

	// Anim-state oracle for RC: what asset is actually playing (debugging frozen poses).
	UFUNCTION(BlueprintCallable, Category = "Emerge|Anim")
	FString GetAnimDebug() const;

	// One-shot vault clip for traversal mantles; returns the clip's play length (0 if unavailable)
	// so the controller can match the scripted climb to the animation.
	float PlayMantleAnim();

protected:
	UPROPERTY(EditAnywhere, Category = "Emerge|Anim") TObjectPtr<UAnimSequenceBase> IdleLoop;
	UPROPERTY(EditAnywhere, Category = "Emerge|Anim") TObjectPtr<UAnimSequenceBase> WalkLoop;
	UPROPERTY(EditAnywhere, Category = "Emerge|Anim") TObjectPtr<UAnimSequenceBase> JogLoop;
	UPROPERTY(EditAnywhere, Category = "Emerge|Anim") TObjectPtr<UAnimSequenceBase> MantleClip;

private:
	TObjectPtr<UAnimSequenceBase> CurrentLoop;
};
