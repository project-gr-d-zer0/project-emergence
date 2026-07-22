#include "AI/EmergeDirectorState.h"

int32 UEmergeDirectorState::Step(int32 State, float Intensity, float TimeInState, float DistTraveledUu)
{
	switch (State)
	{
	case 0:
		return Intensity >= 0.8f ? 1 : 0;
	case 1:
		return TimeInState >= 4.f ? 2 : 1;
	case 2:
		return Intensity <= 0.3f ? 3 : 2;
	case 3:
		return (TimeInState >= 35.f || DistTraveledUu >= 15000.f) ? 0 : 3;
	default:
		return 0;
	}
}
