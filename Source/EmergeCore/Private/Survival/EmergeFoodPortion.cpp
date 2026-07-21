#include "Survival/EmergeFoodPortion.h"

float UEmergeFoodPortion::PortionSize(float TotalAmount, int32 Portions)
{
	return Portions > 0 ? (TotalAmount / Portions) : TotalAmount;
}

int32 UEmergeFoodPortion::MaxPortions(float TotalAmount, float MinPortion)
{
	return MinPortion > 0.0f ? (int32)(TotalAmount / MinPortion) : 0;
}

float UEmergeFoodPortion::Remaining(float TotalAmount, float Consumed)
{
	return FMath::Max(0.0f, TotalAmount - Consumed);
}