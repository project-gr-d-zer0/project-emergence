#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeXpToast.generated.h"

UCLASS()
class EMERGECORE_API UEmergeXpToast : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Shell|XP Toast")
	static int32 LevelForXp(int32 Xp, int32 XpPerLevel);

	UFUNCTION(BlueprintCallable, Category = "Shell|XP Toast")
	static float LevelProgress01(int32 Xp, int32 XpPerLevel);

	UFUNCTION(BlueprintCallable, Category = "Shell|XP Toast")
	static bool DidLevelUp(int32 XpBefore, int32 XpAfter, int32 XpPerLevel);
};