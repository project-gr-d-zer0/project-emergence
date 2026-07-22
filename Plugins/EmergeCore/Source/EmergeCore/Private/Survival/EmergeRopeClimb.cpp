#include "Survival/EmergeRopeClimb.h"

float UEmergeRopeClimb::StaminaCost(float HeightMeters, float PerMeterCost)
{
    return HeightMeters * PerMeterCost;
}

bool UEmergeRopeClimb::CanClimb(float CurrentStamina, float HeightMeters, float PerMeterCost)
{
    return CurrentStamina >= StaminaCost(HeightMeters, PerMeterCost);
}

float UEmergeRopeClimb::ClimbTimeSec(float HeightMeters, float ClimbSpeedMps)
{
    return HeightMeters / ClimbSpeedMps;
}