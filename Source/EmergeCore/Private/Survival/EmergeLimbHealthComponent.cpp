#include "Survival/EmergeLimbHealthComponent.h"

namespace
{
	constexpr int32 NumLimbs = 7;
}

UEmergeLimbHealthComponent::UEmergeLimbHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	ResetLimbs();
}

float UEmergeLimbHealthComponent::MaxHPForLimb(EEmergeLimb Limb)
{
	switch (Limb)
	{
	case EEmergeLimb::Head: return 35.0f;
	case EEmergeLimb::Thorax: return 85.0f;
	case EEmergeLimb::Stomach: return 70.0f;
	case EEmergeLimb::LeftArm:
	case EEmergeLimb::RightArm: return 60.0f;
	case EEmergeLimb::LeftLeg:
	case EEmergeLimb::RightLeg: return 65.0f;
	default: return 0.0f;
	}
}

float UEmergeLimbHealthComponent::OverflowMultiplier(EEmergeLimb Limb)
{
	switch (Limb)
	{
	case EEmergeLimb::LeftArm:
	case EEmergeLimb::RightArm: return 0.49f;
	case EEmergeLimb::LeftLeg:
	case EEmergeLimb::RightLeg: return 0.7f;
	case EEmergeLimb::Stomach: return 1.05f;
	default: return 1.0f;
	}
}

bool UEmergeLimbHealthComponent::IsFatalLimb(EEmergeLimb Limb)
{
	return Limb == EEmergeLimb::Head || Limb == EEmergeLimb::Thorax;
}

float UEmergeLimbHealthComponent::GetHP(EEmergeLimb Limb) const
{
	const int32 i = (int32)Limb;
	return LimbHP.IsValidIndex(i) ? LimbHP[i] : 0.0f;
}

bool UEmergeLimbHealthComponent::IsBlacked(EEmergeLimb Limb) const
{
	return GetHP(Limb) <= 0.0f;
}

bool UEmergeLimbHealthComponent::IsDead() const
{
	return IsBlacked(EEmergeLimb::Head) || IsBlacked(EEmergeLimb::Thorax);
}

bool UEmergeLimbHealthComponent::BothLegsBlacked() const
{
	return IsBlacked(EEmergeLimb::LeftLeg) && IsBlacked(EEmergeLimb::RightLeg);
}

float UEmergeLimbHealthComponent::TotalHP() const
{
	float Total = 0.0f;
	for (float V : LimbHP)
	{
		Total += V;
	}
	return Total;
}

void UEmergeLimbHealthComponent::ApplyDamageToLimb(EEmergeLimb Limb, float Damage)
{
	const int32 Hit = (int32)Limb;
	if (!LimbHP.IsValidIndex(Hit) || Damage <= 0.0f)
	{
		return;
	}

	const float Applied = FMath::Min(Damage, LimbHP[Hit]);
	LimbHP[Hit] -= Applied;
	const float Excess = Damage - Applied;

	// Fatal limbs don't overflow — at 0 the body is dead anyway.
	if (Excess <= 0.0f || IsFatalLimb(Limb))
	{
		return;
	}

	// Overflow: excess * limb multiplier, split evenly across the other surviving limbs.
	const float Overflow = Excess * OverflowMultiplier(Limb);
	TArray<int32> Survivors;
	for (int32 i = 0; i < LimbHP.Num(); ++i)
	{
		if (i != Hit && LimbHP[i] > 0.0f)
		{
			Survivors.Add(i);
		}
	}
	if (Survivors.Num() == 0)
	{
		return;
	}
	const float Share = Overflow / Survivors.Num();
	for (int32 i : Survivors)
	{
		LimbHP[i] = FMath::Max(0.0f, LimbHP[i] - Share);
	}
}

void UEmergeLimbHealthComponent::ResetLimbs()
{
	LimbHP.SetNum(NumLimbs);
	for (int32 i = 0; i < NumLimbs; ++i)
	{
		LimbHP[i] = MaxHPForLimb((EEmergeLimb)i);
	}
}
