#include "Survival/EmergeSurvivorTick.h"
#include "Survival/EmergeMetabolism.h"
#include "Survival/EmergeBleeding.h"
#include "Survival/EmergeDisease.h"

float UEmergeSurvivorTick::DrainedEnergy(float Energy, int32 Movement, float DeltaT)
{
	return FMath::Clamp(Energy - UEmergeMetabolism::DrainPerTick(Movement, Energy, DeltaT), 0.0f, 5000.0f);
}

float UEmergeSurvivorTick::DrainedBlood(float Blood, int32 Intensity, int32 Sources, float DeltaT)
{
	return FMath::Clamp(Blood + Sources * UEmergeBleeding::SourceBloodLossPerTick(Intensity, Blood, DeltaT), 0.0f, 5000.0f);
}

int32 UEmergeSurvivorTick::ImmunityFromState(float Energy, float Water, float Health01, float Blood01)
{
	return UEmergeDisease::ImmunityLevel(UEmergeDisease::Immunity01(Energy, Water, Health01, Blood01));
}

bool UEmergeSurvivorTick::IsDeteriorating(float EnergyBefore, float EnergyAfter, float BloodBefore, float BloodAfter)
{
	return (EnergyAfter < EnergyBefore) || (BloodAfter < BloodBefore);
}