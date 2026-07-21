#include "Survival/EmergeShelter.h"

bool UEmergeShelter::CanBuild(int32 MaterialsHave, int32 MaterialsReq)
{
	return MaterialsHave >= MaterialsReq;
}

float UEmergeShelter::BuildProgressPerTick(bool bBuilding, float BuildSpeed, float DeltaT)
{
	return bBuilding ? (BuildSpeed * DeltaT) : 0.0f;
}

bool UEmergeShelter::IsComplete(float Progress, float Required)
{
	return Progress >= Required;
}