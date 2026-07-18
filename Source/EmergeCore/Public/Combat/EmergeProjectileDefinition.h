#pragma once
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "EmergeProjectileDefinition.generated.h"

// Minimal runtime projectile data used by penetration/mitigation resolution.
UCLASS(BlueprintType)
class EMERGECORE_API UEmergeProjectileDefinition : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
	float BaseDamage = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
	float Penetration = 0.0f;
};
