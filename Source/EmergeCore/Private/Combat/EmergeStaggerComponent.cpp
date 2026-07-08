#include "Combat/EmergeStaggerComponent.h"

UEmergeStaggerComponent::UEmergeStaggerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UEmergeStaggerComponent::ApplyHit(float Damage, EEmergeBodyPart Part, EEmergeHitDir Dir)
{
	Meter += UEmergeStagger::HitStaggerAdd(Damage, Part, Dir);
}

void UEmergeStaggerComponent::Recover(float DeltaSeconds)
{
	Meter = UEmergeStagger::MeterAfterRecovery(Meter, RecoveryPerSec, DeltaSeconds);
}

EEmergeStaggerState UEmergeStaggerComponent::CurrentState() const
{
	return UEmergeStagger::StateForMeter(Meter);
}

float UEmergeStaggerComponent::SpeedMultiplier() const
{
	return UEmergeStagger::LocomotionSpeedMultiplier(CurrentState());
}

void UEmergeStaggerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	Recover(DeltaTime);
}
