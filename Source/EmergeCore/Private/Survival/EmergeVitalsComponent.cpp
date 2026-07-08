#include "Survival/EmergeVitalsComponent.h"
#include "Survival/EmergeVitals.h"

UEmergeVitalsComponent::UEmergeVitalsComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UEmergeVitalsComponent::Simulate(float DeltaSeconds)
{
	Hydration = UEmergeVitals::Deplete(Hydration, HydrationDrainPerSec, DeltaSeconds);
	const float Dmg = UEmergeVitals::StarvationDamage(Hydration, DeltaSeconds, StarveDamagePerSec);
	Health = FMath::Clamp(Health - Dmg, 0.0f, 100.0f);
}

bool UEmergeVitalsComponent::IsStarving() const
{
	return UEmergeVitals::IsStarving(Hydration);
}

void UEmergeVitalsComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	Simulate(DeltaTime);
}
