#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeKillfeed.generated.h"

UCLASS()
class EMERGECORE_API UEmergeKillfeed : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Shell|Killfeed")
	static bool IsHeadshotKill(int32 KillFlags);

	UFUNCTION(BlueprintCallable, Category = "Shell|Killfeed")
	static bool IsWallbang(int32 KillFlags);

	UFUNCTION(BlueprintCallable, Category = "Shell|Killfeed")
	static float EntryLifetimeSec(bool bImportant);
};