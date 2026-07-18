#pragma once
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "EmergeArmorItem.generated.h"

// Minimal runtime armor data used by penetration/mitigation resolution.
UCLASS(BlueprintType)
class EMERGECORE_API UEmergeArmorItem : public UObject
{
	GENERATED_BODY()
public:
	// Penetration threshold; a hit with Penetration >= ResistanceTier is considered penetrated.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Armor")
	float ResistanceTier = 0.0f;

	// Fractional damage mitigation in [0,1] applied when a hit is NOT penetrated.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Armor", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float Mitigation = 0.0f;
};
