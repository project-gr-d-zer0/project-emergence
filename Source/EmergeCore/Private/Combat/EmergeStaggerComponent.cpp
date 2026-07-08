#include "Combat/EmergeStaggerComponent.h"

UEmergeStaggerComponent::UEmergeStaggerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UEmergeStaggerComponent::ApplyHit(float Damage, EEmergeBodyPart Part, EEmergeHitDir Dir)
{
	Meter += UEmergeStagger::HitStaggerAddTuned(Tuning, Damage, Part, Dir);
}

void UEmergeStaggerComponent::Recover(float DeltaSeconds)
{
	Meter = UEmergeStagger::MeterAfterRecovery(Meter, RecoveryPerSec, DeltaSeconds);
}

EEmergeStaggerState UEmergeStaggerComponent::CurrentState() const
{
	return UEmergeStagger::StateForMeterTuned(Tuning, Meter);
}

float UEmergeStaggerComponent::SpeedMultiplier() const
{
	return UEmergeStagger::LocomotionSpeedMultiplierTuned(Tuning, CurrentState());
}

void UEmergeStaggerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	Recover(DeltaTime);
}
