#include "Nav/EmergeFlock.h"

FVector UEmergeFlock::Separation(FVector SelfLocation, FVector Neighbor, float DesiredDist)
{
	const FVector D = SelfLocation - Neighbor;
	const float Dist = D.Size();
	if (Dist <= 0.0f || Dist >= DesiredDist)
	{
		return FVector::ZeroVector;
	}
	return D.GetSafeNormal() * (1.0f - Dist / DesiredDist);
}

FVector UEmergeFlock::Cohesion(FVector SelfLocation, FVector GroupCenter)
{
	const FVector D = GroupCenter - SelfLocation;
	return D.IsNearlyZero() ? FVector::ZeroVector : D.GetSafeNormal();
}

FVector UEmergeFlock::Alignment(FVector AvgHeading)
{
	return AvgHeading.IsNearlyZero() ? FVector::ZeroVector : AvgHeading.GetSafeNormal();
}

FVector UEmergeFlock::Combine(FVector Sep, FVector Coh, FVector Ali, FVector Seek, float WSep, float WCoh, float WAli, float WSeek)
{
	const FVector Sum = Sep * WSep + Coh * WCoh + Ali * WAli + Seek * WSeek;
	return Sum.IsNearlyZero() ? FVector::ZeroVector : Sum.GetSafeNormal();
}
