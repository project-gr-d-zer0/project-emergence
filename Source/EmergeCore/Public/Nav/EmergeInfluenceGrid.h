#pragma once
#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "EmergeInfluenceGrid.generated.h"

// Shared, decaying knowledge grid: the herd's belief of where the player is. Enemies STAMP it when they see
// the player; it exponentially decays + spatially diffuses over time (the search area spreads as info ages),
// so the shared target drifts to a stale spot and disperses when the player outruns them. One grid per world,
// read/written by every enemy -> unifies the single chase and the Mass horde. (Isla occupancy-map pattern.)
UCLASS()
class EMERGECORE_API UEmergeInfluenceGrid : public UTickableWorldSubsystem
{
	GENERATED_BODY()
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;
	virtual bool IsTickable() const override { return bReady; }

	// Enemy calls this when it sees the player -> writes the shared belief.
	UFUNCTION(BlueprintCallable, Category = "Emerge|Influence") void Stamp(FVector WorldPos, float Strength);
	UFUNCTION(BlueprintPure, Category = "Emerge|Influence") float Sample(FVector WorldPos) const;
	UFUNCTION(BlueprintPure, Category = "Emerge|Influence") FVector PeakPosition() const;
	UFUNCTION(BlueprintPure, Category = "Emerge|Influence") float PeakConfidence() const;
	UFUNCTION(BlueprintPure, Category = "Emerge|Influence") FVector GradientAt(FVector WorldPos) const;
	UFUNCTION(BlueprintPure, Category = "Emerge|Influence") bool IsDispersed() const;
	UFUNCTION(BlueprintCallable, Category = "Emerge|Influence") FString GetGridStatus();

protected:
	UPROPERTY(EditAnywhere, Category = "Emerge|Influence") int32 Dims = 100;
	UPROPERTY(EditAnywhere, Category = "Emerge|Influence") float CellSize = 300.0f;
	UPROPERTY(EditAnywhere, Category = "Emerge|Influence") FVector Center = FVector::ZeroVector;
	UPROPERTY(EditAnywhere, Category = "Emerge|Influence") float Tau = 12.0f;         // decay time-constant (s)
	UPROPERTY(EditAnywhere, Category = "Emerge|Influence") float DiffuseRate = 0.03f; // spatial blur per tick (0.12@60fps emptied the belief in ~6s — measured 2026-07-09)
	UPROPERTY(EditAnywhere, Category = "Emerge|Influence") float DispersalFloor = 0.05f;

private:
	TArray<float> Cells;
	TArray<float> Scratch;
	bool bReady = false;
	float Staleness = 0.0f;
	int32 PeakIdx = INDEX_NONE;
	FORCEINLINE int32 Idx(int32 X, int32 Y) const { return Y * Dims + X; }
	bool WorldToCell(const FVector& P, int32& OutX, int32& OutY) const;
	FVector CellToWorld(int32 X, int32 Y) const;
};
