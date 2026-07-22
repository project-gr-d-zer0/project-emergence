#include "Nav/EmergeCornering.h"

float UEmergeCornering::SpeedScale(float TurnErrorDeg, float StartDeg, float FullDeg, float MinScale)
{
	const float Err = FMath::Abs(TurnErrorDeg);
	const float Min = FMath::Clamp(MinScale, 0.0f, 1.0f);
	if (FullDeg <= StartDeg)
	{
		return (Err >= FullDeg) ? Min : 1.0f;
	}
	const float T = FMath::Clamp((Err - StartDeg) / (FullDeg - StartDeg), 0.0f, 1.0f);
	return FMath::Lerp(1.0f, Min, T);
}
