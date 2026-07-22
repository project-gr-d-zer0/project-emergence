#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeHealthGrade.generated.h"

UCLASS()
class EMERGECORE_API UEmergeHealthGrade : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Survival|Health")
	static FString GradeFor(float Health);
};