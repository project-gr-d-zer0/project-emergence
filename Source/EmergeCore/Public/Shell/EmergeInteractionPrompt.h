#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeInteractionPrompt.generated.h"

UCLASS()
class EMERGECORE_API UEmergeInteractionPrompt : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Shell|InteractionPrompt")
	static bool InRange(float Distance, float MaxRange);

	UFUNCTION(BlueprintCallable, Category = "Shell|InteractionPrompt")
	static bool IsHigherPriority(float DistA, int32 PrioA, float DistB, int32 PrioB);

	UFUNCTION(BlueprintCallable, Category = "Shell|InteractionPrompt")
	static float PromptAlpha(float Distance, float MaxRange);
};