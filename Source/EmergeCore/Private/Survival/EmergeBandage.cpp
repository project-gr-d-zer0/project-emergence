#include "Survival/EmergeBandage.h"

int32 UEmergeBandage::SourcesClosedPerApplication()
{
	return 1;
}

int32 UEmergeBandage::RemainingSources(int32 SourcesBefore)
{
	return FMath::Max(0, SourcesBefore - 1);
}

bool UEmergeBandage::StopsAllBleeding(int32 SourcesBefore)
{
	return SourcesBefore <= 1;
}

float UEmergeBandage::BandageTimeSec()
{
	return 4.0f;
}

float UEmergeBandage::DressingInfectionChance(bool bClean)
{
	return bClean ? 0.0f : 0.05f;
}

int32 UEmergeBandage::QuantityConsumed()
{
	return 1;
}