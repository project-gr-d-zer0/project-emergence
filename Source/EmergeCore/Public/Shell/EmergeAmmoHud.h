#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeAmmoHud.generated.h"

UCLASS()
class EMERGECORE_API UEmergeAmmoHud : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Shell|AmmoHud")
	static float MagFill01(int32 Current, int32 MagSize);

	UFUNCTION(BlueprintCallable, Category = "Shell|AmmoHud")
	static bool IsEmpty(int32 Current);

	UFUNCTION(BlueprintCallable, Category = "Shell|AmmoHud")
	static bool LowAmmo(int32 Current, int32 MagSize);

	UFUNCTION(BlueprintCallable, Category = "Shell|AmmoHud")
	static int32 MagAfterReload(int32 Reserve, int32 Current, int32 MagSize);

	UFUNCTION(BlueprintCallable, Category = "Shell|AmmoHud")
	static int32 ReserveAfterReload(int32 Reserve, int32 Current, int32 MagSize);
};