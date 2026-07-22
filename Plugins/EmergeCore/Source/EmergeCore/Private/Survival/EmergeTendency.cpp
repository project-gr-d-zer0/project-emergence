#include "Survival/EmergeTendency.h"

int32 UEmergeTendency::TendencyLevel(float Delta)
{
	float AbsDelta = FMath::Abs(Delta);

	if (AbsDelta < 0.5f)
	{
		return 0;
	}
	else if (AbsDelta < 2.0f)
	{
		return Delta > 0 ? 1 : -1;
	}
	else if (AbsDelta < 5.0f)
	{
		return Delta > 0 ? 2 : -2;
	}
	else
	{
		return Delta > 0 ? 3 : -3;
	}
}