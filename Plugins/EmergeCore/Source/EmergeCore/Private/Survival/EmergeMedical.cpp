#include "Survival/EmergeMedical.h"

float UEmergeMedical::BleedDamage(float BleedRate, float DeltaSeconds, bool bBleeding)
{
	return bBleeding ? BleedRate * FMath::Max(0.0f, DeltaSeconds) : 0.0f;
}

float UEmergeMedical::ApplyHeal(float CurrentHP, float HealAmount)
{
	return FMath::Clamp(CurrentHP + FMath::Max(0.0f, HealAmount), 0.0f, 100.0f);
}

bool UEmergeMedical::StopsBleeding(int32 ItemType)
{
	return ItemType == 1 || ItemType == 2;
}
