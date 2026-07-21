#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeRecoil.generated.h"

UCLASS()
class EMERGECORE_API UEmergeRecoil : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Combat|Recoil")
	static float VerticalKick(int32 ShotIndex, float BasePitch, float ClimbPerShot, float MaxPitch);

	UFUNCTION(BlueprintCallable, Category = "Combat|Recoil")
	static float HorizontalKick(int32 ShotIndex, float BaseYaw);

	UFUNCTION(BlueprintCallable, Category = "Combat|Recoil")
	static float RecoverToward(float Current, float RecoverStep);
};