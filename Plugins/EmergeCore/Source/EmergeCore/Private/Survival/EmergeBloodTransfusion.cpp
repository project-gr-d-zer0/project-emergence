#include "Survival/EmergeBloodTransfusion.h"

bool UEmergeBloodTransfusion::IsCompatible(int32 DonorAbo, bool bDonorRhPos, int32 RecipAbo, bool bRecipRhPos)
{
	return ((DonorAbo & RecipAbo) == DonorAbo) && (bRecipRhPos || !bDonorRhPos);
}

int32 UEmergeBloodTransfusion::TransfusionReaction(bool bCompatible, float BloodAmount)
{
	if (bCompatible) return 0;
	if (BloodAmount > 175.0f) return 2; // HEMOLYTIC_REACTION_THRESHOLD
	if (BloodAmount > 75.0f)  return 1; // HEMOLYTIC_RISK_SHOCK_THRESHOLD
	return 0;
}

float UEmergeBloodTransfusion::HemolyticDrainPerTick(float DeltaT)
{
	return 7.0f * DeltaT; // HEMOLYTIC_BLOOD_DRAIN_PER_SEC
}