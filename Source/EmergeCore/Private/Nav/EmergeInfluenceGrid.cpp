#include "Nav/EmergeInfluenceGrid.h"

void UEmergeInfluenceGrid::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	Dims = FMath::Max(8, Dims);
	Cells.Init(0.0f, Dims * Dims);
	Scratch.Init(0.0f, Dims * Dims);
	bReady = true;
}

TStatId UEmergeInfluenceGrid::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UEmergeInfluenceGrid, STATGROUP_Tickables);
}

bool UEmergeInfluenceGrid::WorldToCell(const FVector& P, int32& OutX, int32& OutY) const
{
	const float Half = 0.5f * Dims * CellSize;
	OutX = FMath::FloorToInt((P.X - Center.X + Half) / CellSize);
	OutY = FMath::FloorToInt((P.Y - Center.Y + Half) / CellSize);
	return (OutX >= 0 && OutX < Dims && OutY >= 0 && OutY < Dims);
}

FVector UEmergeInfluenceGrid::CellToWorld(int32 X, int32 Y) const
{
	const float Half = 0.5f * Dims * CellSize;
	return FVector(Center.X - Half + (X + 0.5f) * CellSize, Center.Y - Half + (Y + 0.5f) * CellSize, Center.Z);
}

void UEmergeInfluenceGrid::Stamp(FVector WorldPos, float Strength)
{
	int32 X, Y;
	if (!bReady || !WorldToCell(WorldPos, X, Y)) { return; }
	const int32 I = Idx(X, Y);
	Cells[I] = FMath::Clamp(FMath::Max(Cells[I], Strength), 0.0f, 1.0f);
	Staleness = 0.0f;
}

void UEmergeInfluenceGrid::Tick(float DeltaTime)
{
	if (!bReady || Cells.Num() != Dims * Dims) { return; }
	const float DecayMul = (Tau > 0.0f) ? FMath::Exp(-DeltaTime / Tau) : 1.0f;
	const float R = FMath::Clamp(DiffuseRate, 0.0f, 1.0f);
	float BestVal = 0.0f;
	int32 BestIdx = INDEX_NONE;
	for (int32 Y = 0; Y < Dims; ++Y)
	{
		for (int32 X = 0; X < Dims; ++X)
		{
			const int32 I = Idx(X, Y);
			float Sum = 0.0f; int32 N = 0;
			if (X > 0)      { Sum += Cells[I - 1];    ++N; }
			if (X < Dims-1) { Sum += Cells[I + 1];    ++N; }
			if (Y > 0)      { Sum += Cells[I - Dims]; ++N; }
			if (Y < Dims-1) { Sum += Cells[I + Dims]; ++N; }
			const float NeighborAvg = (N > 0) ? Sum / (float)N : Cells[I];
			float V = FMath::Lerp(Cells[I], NeighborAvg, R) * DecayMul;   // diffuse (spread) then forget
			if (V < 0.0005f) { V = 0.0f; }
			Scratch[I] = V;
			if (V > BestVal) { BestVal = V; BestIdx = I; }
		}
	}
	Swap(Cells, Scratch);
	PeakIdx = BestIdx;
	Staleness += DeltaTime;
}

float UEmergeInfluenceGrid::Sample(FVector WorldPos) const
{
	int32 X, Y;
	if (!bReady || !WorldToCell(WorldPos, X, Y)) { return 0.0f; }
	return Cells[Idx(X, Y)];
}

FVector UEmergeInfluenceGrid::PeakPosition() const
{
	if (PeakIdx == INDEX_NONE) { return Center; }
	return CellToWorld(PeakIdx % Dims, PeakIdx / Dims);
}

float UEmergeInfluenceGrid::PeakConfidence() const
{
	return (PeakIdx == INDEX_NONE) ? 0.0f : Cells[PeakIdx];
}

FVector UEmergeInfluenceGrid::GradientAt(FVector WorldPos) const
{
	int32 X, Y;
	if (!bReady || !WorldToCell(WorldPos, X, Y)) { return FVector::ZeroVector; }
	const float L  = (X > 0)      ? Cells[Idx(X - 1, Y)] : 0.0f;
	const float Rr = (X < Dims-1) ? Cells[Idx(X + 1, Y)] : 0.0f;
	const float D  = (Y > 0)      ? Cells[Idx(X, Y - 1)] : 0.0f;
	const float U  = (Y < Dims-1) ? Cells[Idx(X, Y + 1)] : 0.0f;
	const FVector G(Rr - L, U - D, 0.0f);
	return G.IsNearlyZero() ? FVector::ZeroVector : G.GetSafeNormal();
}

bool UEmergeInfluenceGrid::IsDispersed() const
{
	return PeakConfidence() < DispersalFloor;
}

FString UEmergeInfluenceGrid::GetGridStatus()
{
	const FVector P = PeakPosition();
	return FString::Printf(TEXT("{\"peak\":[%.0f,%.0f],\"peakConf\":%.3f,\"staleness\":%.1f,\"dispersed\":%s}"),
		P.X, P.Y, PeakConfidence(), Staleness, IsDispersed() ? TEXT("true") : TEXT("false"));
}
