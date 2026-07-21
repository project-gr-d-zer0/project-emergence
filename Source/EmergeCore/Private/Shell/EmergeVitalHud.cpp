#include "Shell/EmergeVitalHud.h"

float UEmergeVitalHud::BarFill01(float Value, float Max)
{
	return FMath::Clamp(Value / Max, 0.0f, 1.0f);
}

int32 UEmergeVitalHud::BloodStatLevel(float Blood)
{
	if (Blood <= 3000.0f) return 4;
	if (Blood <= 3500.0f) return 3;
	if (Blood <= 4000.0f) return 2;
	if (Blood <= 4500.0f) return 1;
	return 0;
}

int32 UEmergeVitalHud::HealthStatLevel(float Health)
{
	if (Health <= 15.0f) return 4;
	if (Health <= 30.0f) return 3;
	if (Health <= 50.0f) return 2;
	if (Health <= 80.0f) return 1;
	return 0;
}

int32 UEmergeVitalHud::EnergyStatLevel(float Energy)
{
	if (Energy <= 0.0f) return 4;
	if (Energy <= 300.0f) return 3;
	if (Energy <= 800.0f) return 2;
	if (Energy <= 3500.0f) return 1;
	return 0;
}

int32 UEmergeVitalHud::WaterStatLevel(float Water)
{
	if (Water <= 0.0f) return 4;
	if (Water <= 300.0f) return 3;
	if (Water <= 800.0f) return 2;
	if (Water <= 3500.0f) return 1;
	return 0;
}

int32 UEmergeVitalHud::DisplayColorTier(int32 StatLevel)
{
	if (StatLevel >= 4) return 3;
	if (StatLevel == 3) return 2;
	if (StatLevel == 2) return 1;
	return 0;
}

bool UEmergeVitalHud::ShowLowVitalBadge(int32 StatLevel)
{
	return StatLevel >= 3;
}