#include "Nav/EmergeStealthFill.h"

float UEmergeStealthFill::StanceMultiplier(int32 Stance)
{
	switch (Stance)
	{
	case 0: return 1.0f;
	case 1: return 0.6f;
	case 2: return 0.35f;
	default: return 1.0f;
	}
}

float UEmergeStealthFill::LightMultiplier(float Light01)
{
	return FMath::Clamp(Light01, 0.2f, 1.f);
}

float UEmergeStealthFill::MotionMultiplier(float Speed01)
{
	return 1.f + FMath::Clamp(Speed01, 0.f, 1.f);
}

float UEmergeStealthFill::FillMultiplier(int32 Stance, float Light01, float Speed01)
{
	return StanceMultiplier(Stance) * LightMultiplier(Light01) * MotionMultiplier(Speed01);
}

bool UEmergeStealthFill::PassesVisibilityGate(float VisibleFraction)
{
	return VisibleFraction >= 0.25f;
}
