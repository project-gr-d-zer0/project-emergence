#include "Survival/EmergeCookState.h"

int32 UEmergeCookState::CookStage(float CookProgress, float CookedAt, float BurnedAt)
{
	if (CookProgress >= BurnedAt) return 2;
	if (CookProgress >= CookedAt) return 1;
	return 0;
}

float UEmergeCookState::CookProgressPerTick(bool bOnHeat, float DeltaT)
{
	return bOnHeat ? DeltaT : 0.0f;
}