#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmergeSensorMath.generated.h"

// Pure, deterministic decision math extracted from AEmergeCharacter::SenseEnvironment so the
// affordance/consequence/normalization/occupancy-grid logic is unit-testable headless.
UCLASS()
class EMERGECORE_API UEmergeSensorMath : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	// Clear to walk forward: no near wall, not a ledge, drop not too big.
	UFUNCTION(BlueprintCallable, Category = "Sensor")
	static bool CanMoveForward(float WallAheadD, float FloorDropAhead, bool bLedge);

	// 1 = blocked (wall within 100), else 2 = dropoff (ledge or drop > 200), else 0 = clear.
	UFUNCTION(BlueprintCallable, Category = "Sensor")
	static int32 ConsequenceCode(float WallAheadD, float FloorDropAhead, bool bLedge);

	UFUNCTION(BlueprintCallable, Category = "Sensor")
	static float NormalizeClamped(float Value, float MaxValue);

	UFUNCTION(BlueprintCallable, Category = "Sensor")
	static int64 PackCell(int32 CellX, int32 CellY);

	UFUNCTION(BlueprintCallable, Category = "Sensor")
	static int32 UnpackCellX(int64 Key);

	UFUNCTION(BlueprintCallable, Category = "Sensor")
	static int32 UnpackCellY(int64 Key);
};
