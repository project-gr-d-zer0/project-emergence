#include "Survival/EmergeSurvivalNeedsComponent.h"
#include "Survival/EmergeSurvivorTick.h"

void UEmergeSurvivalNeedsComponent::Simulate(float DeltaSeconds)
{
	Energy = UEmergeSurvivorTick::DrainedEnergy(Energy, Movement, DeltaSeconds);
	Water  = UEmergeSurvivorTick::DrainedEnergy(Water,  Movement, DeltaSeconds);
	Blood  = UEmergeSurvivorTick::DrainedBlood(Blood, 2, BleedingSources, DeltaSeconds);
}

bool UEmergeSurvivalNeedsComponent::IsAlive() const
{
	return Blood > 2500.0f;  // BLOOD_THRESHOLD_FATAL
}

int32 UEmergeSurvivalNeedsComponent::ImmunityLevel() const
{
	return UEmergeSurvivorTick::ImmunityFromState(Energy, Water, 1.0f, Blood / 5000.0f);
}

void UEmergeSurvivalNeedsComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Simulate(DeltaTime);
}