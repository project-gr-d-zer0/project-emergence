#include "Combat/EmergeDamageComponent.h"
#include "Combat/EmergeBallistics.h"
#include "Survival/EmergeVitalsComponent.h"
#include "Combat/EmergeStaggerComponent.h"

UEmergeDamageComponent::UEmergeDamageComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

FEmergeHitResult UEmergeDamageComponent::ResolveHit(float RawDamage, float PenetrationValue, int32 ArmorClass,
	float ArmorValue, EEmergeBodyPart Part, EEmergeHitDir Dir)
{
	FEmergeHitResult Result;
	Result.bPenetrated = UEmergeBallistics::Penetrates(PenetrationValue, ArmorClass);
	Result.FinalDamage = UEmergeBallistics::MitigatedDamage(RawDamage, ArmorValue, Result.bPenetrated);
	Result.StaggerAdd = UEmergeStagger::HitStaggerAdd(Result.FinalDamage, Part, Dir);
	return Result;
}

void UEmergeDamageComponent::ApplyHit(const FEmergeHitResult& Hit, UEmergeVitalsComponent* Vitals, UEmergeStaggerComponent* Stagger)
{
	if (Vitals)
	{
		Vitals->Health = FMath::Clamp(Vitals->Health - Hit.FinalDamage, 0.0f, 100.0f);
	}
	if (Stagger)
	{
		Stagger->Meter += Hit.StaggerAdd;
	}
}
