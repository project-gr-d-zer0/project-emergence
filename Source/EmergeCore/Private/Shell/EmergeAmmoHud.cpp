#include "Shell/EmergeAmmoHud.h"

float UEmergeAmmoHud::MagFill01(int32 Current, int32 MagSize)
{
	return FMath::Clamp((float)Current / (float)MagSize, 0.f, 1.f);
}

bool UEmergeAmmoHud::IsEmpty(int32 Current)
{
	return Current <= 0;
}

bool UEmergeAmmoHud::LowAmmo(int32 Current, int32 MagSize)
{
	return Current > 0 && (Current * 4) <= MagSize; // <=25%, not empty
}

int32 UEmergeAmmoHud::MagAfterReload(int32 Reserve, int32 Current, int32 MagSize)
{
	const int32 Taken = FMath::Min(MagSize - Current, Reserve);
	return Current + Taken;
}

int32 UEmergeAmmoHud::ReserveAfterReload(int32 Reserve, int32 Current, int32 MagSize)
{
	const int32 Taken = FMath::Min(MagSize - Current, Reserve);
	return Reserve - Taken;
}