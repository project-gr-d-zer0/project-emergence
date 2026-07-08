#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeWeapon.generated.h"

// Deterministic weapon fire stats: cadence, magazine tracking, and climbing recoil.
UCLASS()
class EMERGECORE_API UEmergeWeapon : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	// Seconds between shots for a given rate of fire, guarding against zero/negative RPM.
	UFUNCTION(BlueprintCallable, Category = "Combat|Weapon")
	static float TimeBetweenShots(int32 RoundsPerMinute);

	// Rounds remaining in the magazine after firing, floored at 0.
	UFUNCTION(BlueprintCallable, Category = "Combat|Weapon")
	static int32 MagazineRemaining(int32 Loaded, int32 Fired);

	// Recoil at a given shot index within a burst, climbing 10% per shot.
	UFUNCTION(BlueprintCallable, Category = "Combat|Weapon")
	static float RecoilAtShot(float BaseRecoil, int32 ShotIndex);
};
