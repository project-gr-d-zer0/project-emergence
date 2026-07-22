#include "Survival/EmergeStatusEffectComponent.h"
#include "Survival/EmergeMedical.h"
#include "Survival/EmergeVitalsComponent.h"

UEmergeStatusEffectComponent::UEmergeStatusEffectComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UEmergeStatusEffectComponent::StartBleeding(float RatePerSec)
{
	bBleeding = true;
	BleedRatePerSec = FMath::Max(0.0f, RatePerSec);
}

bool UEmergeStatusEffectComponent::ApplyMedical(int32 ItemType)
{
	if (UEmergeMedical::StopsBleeding(ItemType))
	{
		bBleeding = false;
		return true;
	}
	return false;
}

float UEmergeStatusEffectComponent::Simulate(float DeltaSeconds, UEmergeVitalsComponent* Vitals)
{
	const float Dmg = UEmergeMedical::BleedDamage(BleedRatePerSec, DeltaSeconds, bBleeding);
	if (Vitals && Dmg > 0.0f)
	{
		Vitals->Health = FMath::Clamp(Vitals->Health - Dmg, 0.0f, 100.0f);
	}
	return Dmg;
}
