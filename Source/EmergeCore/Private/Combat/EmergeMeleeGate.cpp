#include "Combat/EmergeMeleeGate.h"

bool UEmergeMeleeGate::InAttackRange(float DistUu, float MinUu, float MaxUu)
{
	return MinUu <= DistUu && DistUu <= MaxUu;
}

bool UEmergeMeleeGate::PassesFacingGate(float DotToTarget, float MinDot)
{
	return DotToTarget >= MinDot;
}

bool UEmergeMeleeGate::BudgetAllows(int32 ActiveWeight, int32 RequestWeight, int32 Capacity)
{
	return (ActiveWeight + RequestWeight) <= Capacity;
}

float UEmergeMeleeGate::HitstopSeconds(bool bHeavy)
{
	return bHeavy ? (10.f / 60.f) : (4.f / 60.f);
}
