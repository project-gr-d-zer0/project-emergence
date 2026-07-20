#include "Survival/EmergeFoodSpoilage.h"

int32 UEmergeFoodSpoilage::FreshnessState(float Age, float OffAge, float OffAgeMax)
{
	if (Age >= OffAgeMax)
	{
		return 2;
	}
	if (Age < OffAge)
	{
		return 0;
	}
	return 1;
}

float UEmergeFoodSpoilage::RotSpeedMult(int32 FoodRotSpeedPreset)
{
	switch (FoodRotSpeedPreset)
	{
	case 1: return 1.7f;
	case 2: return 1.4f;
	case 4: return 0.7f;
	case 5: return 0.4f;
	default: return 1.0f;
	}
}

float UEmergeFoodSpoilage::StorageRateMult(int32 StorageKind, bool bPowered)
{
	if (StorageKind == 1)
	{
		return bPowered ? 0.2f : 1.0f;
	}
	if (StorageKind == 2)
	{
		return bPowered ? 0.02f : 1.0f;
	}
	return 1.0f;
}

float UEmergeFoodSpoilage::AgeDeltaDays(float ElapsedHours, int32 StorageKind, bool bPowered, int32 FoodRotSpeedPreset)
{
	return ElapsedHours * StorageRateMult(StorageKind, bPowered) * RotSpeedMult(FoodRotSpeedPreset) / 24.0f;
}
