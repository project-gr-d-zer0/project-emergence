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

	// Accumulated heat after firing Shots additional rounds, each adding HeatPerShot.
	UFUNCTION(BlueprintCallable, Category = "Combat|Weapon")
	static float HeatAfterShots(float Heat, int32 Shots, float HeatPerShot);

	// Heat after cooling for DeltaSeconds at CoolPerSec, floored at 0.
	UFUNCTION(BlueprintCallable, Category = "Combat|Weapon")
	static float HeatAfterCooling(float Heat, float CoolPerSec, float DeltaSeconds);

	// Malfunction chance in [0,1]: zero above 93% durability while cool, rising with wear and heat.
	UFUNCTION(BlueprintCallable, Category = "Combat|Weapon")
	static float MalfunctionChance(float DurabilityPct, float Heat);

	// Spread (MOA) at a given range, growing 1% per meter; aiming halves the spread.
	UFUNCTION(BlueprintCallable, Category = "Combat|Weapon")
	static float SpreadAtRange(float BaseMOA, float RangeM, bool bAiming);
};
