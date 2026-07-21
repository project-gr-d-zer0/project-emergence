#include "Shell/EmergeStaminaHud.h"

float UEmergeStaminaHud::BarFill01(float Current, float MaxStamina)
{
	return FMath::Clamp(Current / MaxStamina, 0.f, 1.f);
}

bool UEmergeStaminaHud::CanPerform(float Current, float Threshold)
{
	return Current >= Threshold;
}

float UEmergeStaminaHud::JumpThreshold()
{
	return 25.0f;
}

float UEmergeStaminaHud::VaultThreshold()
{
	return 20.0f;
}

float UEmergeStaminaHud::ClimbThreshold()
{
	return 42.0f;
}

float UEmergeStaminaHud::HeavyMeleeThreshold()
{
	return 25.0f;
}

float UEmergeStaminaHud::RollThreshold()
{
	return 5.0f;
}

float UEmergeStaminaHud::RegenCooldownSec(bool bExhausted)
{
	return bExhausted ? 0.5f : 0.45f;
}