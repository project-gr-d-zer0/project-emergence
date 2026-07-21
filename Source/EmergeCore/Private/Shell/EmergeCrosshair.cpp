#include "Shell/EmergeCrosshair.h"

float UEmergeCrosshair::SpreadGap(float BaseGap, float MoveSpeed01, bool bFiring)
{
	return BaseGap * (1.0f + MoveSpeed01 + (bFiring ? 0.5f : 0.0f));
}

float UEmergeCrosshair::RecoverGap(float Current, float BaseGap, float RecoverRate, float DeltaT)
{
	return FMath::Max(BaseGap, Current - RecoverRate * DeltaT);
}