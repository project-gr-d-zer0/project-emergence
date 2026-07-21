#include "Combat/EmergeSuppressor.h"

float UEmergeSuppressor::NoiseRadius(float BaseRadius, bool bSuppressed)
{
	return bSuppressed ? BaseRadius * 0.35f : BaseRadius;
}

float UEmergeSuppressor::MuzzleFlashScale(bool bSuppressed)
{
	return bSuppressed ? 0.2f : 1.0f;
}