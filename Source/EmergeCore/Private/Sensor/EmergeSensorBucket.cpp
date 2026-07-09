#include "Sensor/EmergeSensorBucket.h"

int32 UEmergeSensorBucket::BearingSector(float RelX, float RelY)
{
	const float Deg = FMath::RadiansToDegrees(FMath::Atan2(RelY, RelX));
	const int32 S = FMath::RoundToInt(Deg / 45.0f);
	return ((S % 8) + 8) % 8;
}

int32 UEmergeSensorBucket::DistanceBucket(float Dist)
{
	if (Dist < 300.0f)
	{
		return 0;
	}
	if (Dist < 1000.0f)
	{
		return 1;
	}
	return 2;
}
