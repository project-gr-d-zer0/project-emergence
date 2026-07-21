#include "Survival/EmergeWaterBoil.h"

float UEmergeWaterBoil::BoilProgressPerTick(bool bOnHeat, float DeltaT)
{
	return bOnHeat ? DeltaT : 0.0f;
}

bool UEmergeWaterBoil::IsPurified(float BoilProgress, float BoilTimeReq)
{
	return BoilProgress >= BoilTimeReq;
}

float UEmergeWaterBoil::ContaminationAfterBoil(float BaseChance, bool bPurified)
{
	return bPurified ? 0.0f : BaseChance;
}