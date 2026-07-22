#include "Sensor/EmergeSensorMath.h"

bool UEmergeSensorMath::CanMoveForward(float WallAheadD, float FloorDropAhead, bool bLedge)
{
	return (WallAheadD < 0.0f || WallAheadD >= 100.0f) && !bLedge && FloorDropAhead <= 100.0f;
}

int32 UEmergeSensorMath::ConsequenceCode(float WallAheadD, float FloorDropAhead, bool bLedge)
{
	if (WallAheadD >= 0.0f && WallAheadD < 100.0f)
	{
		return 1;
	}
	if (bLedge || FloorDropAhead > 200.0f)
	{
		return 2;
	}
	return 0;
}

float UEmergeSensorMath::NormalizeClamped(float Value, float MaxValue)
{
	return (MaxValue <= 0.0f) ? 0.0f : FMath::Clamp(Value / MaxValue, 0.0f, 1.0f);
}

int64 UEmergeSensorMath::PackCell(int32 CellX, int32 CellY)
{
	return ((int64)CellX << 32) | (int64)((uint32)CellY);
}

int32 UEmergeSensorMath::UnpackCellX(int64 Key)
{
	return (int32)(Key >> 32);
}

int32 UEmergeSensorMath::UnpackCellY(int64 Key)
{
	return (int32)(Key & 0xFFFFFFFF);
}
