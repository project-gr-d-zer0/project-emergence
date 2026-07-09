#include "Sensor/EmergeStuckDetect.h"

float UEmergeStuckDetect::Accumulate(float Current, float Speed, float MinSpeed, bool bExpectedMoving, float DtSeconds)
{
	if (!bExpectedMoving || Speed >= MinSpeed)
	{
		return 0.0f;
	}
	return Current + FMath::Max(0.0f, DtSeconds);
}

bool UEmergeStuckDetect::IsStuck(float StuckSeconds, float ThresholdSeconds)
{
	return StuckSeconds >= FMath::Max(ThresholdSeconds, 0.1f);
}
