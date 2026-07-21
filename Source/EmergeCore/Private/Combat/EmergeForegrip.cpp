#include "Combat/EmergeForegrip.h"

// [CORPUS-FIX] Tarkov foregrips (spt items.json, n=56): recoil only -1..-4%; benefit is ergonomics (-8..+10).
float UEmergeForegrip::RecoilMultiplier(int32 GripType)
{
	switch (GripType)
	{
	case 1: return 0.96f;  // vertical: strongest real foregrip ~ -4%
	case 2: return 0.98f;  // angled: ~ -2%
	case 3: return 1.0f;   // bipod undeployed: no recoil cut (deployed = runtime)
	default: return 1.0f;
	}
}

float UEmergeForegrip::HandlingMultiplier(int32 GripType)
{
	switch (GripType)
	{
	case 1: return 1.05f;  // vertical: mild ergo
	case 2: return 1.1f;   // angled: ergo max +10%
	case 3: return 0.7f;   // bipod hurts mobility
	default: return 1.0f;
	}
}
