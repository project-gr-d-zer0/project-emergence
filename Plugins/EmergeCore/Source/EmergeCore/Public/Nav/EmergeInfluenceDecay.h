#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeInfluenceDecay.generated.h"

// Decaying-knowledge math for the shared influence grid (herd's belief of where the player is).
// Exponential temporal decay + cell diffusion (blur) + expanding search ring + dispersal check.
UCLASS()
class EMERGECORE_API UEmergeInfluenceDecay : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Emerge|Influence")
	static float DecayExp(float Value, float DtSeconds, float Tau);

	UFUNCTION(BlueprintCallable, Category = "Emerge|Influence")
	static float DiffuseCell(float Center, float NeighborAvg, float Rate);

	UFUNCTION(BlueprintCallable, Category = "Emerge|Influence")
	static float SearchRadius(float PlayerSpeed, float StalenessSeconds, float BaseRadius, float MaxRadius);

	UFUNCTION(BlueprintCallable, Category = "Emerge|Influence")
	static bool IsDispersed(float PeakConfidence, float Floor);
};
