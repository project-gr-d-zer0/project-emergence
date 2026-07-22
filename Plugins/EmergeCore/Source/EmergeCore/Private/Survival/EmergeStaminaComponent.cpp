#include "Survival/EmergeStaminaComponent.h"

UEmergeStaminaComponent::UEmergeStaminaComponent()
{
	PrimaryComponentTick.bCanEverTick = false;   // driven by movement/input code, not a blind tick
}

void UEmergeStaminaComponent::Simulate(float DeltaSeconds, bool bSprinting, int32 LoadTier)
{
	const float Dt = FMath::Max(0.0f, DeltaSeconds);
	if (bSprinting)
	{
		const float LoadMult = 1.0f + FMath::Max(0, LoadTier) * 0.5f;
		Stamina -= SprintDrainPerSec * LoadMult * Dt;
	}
	else
	{
		Stamina += RegenPerSec * Dt;
	}
	Stamina = FMath::Clamp(Stamina, 0.0f, 100.0f);
}

bool UEmergeStaminaComponent::CanSprint() const
{
	return Stamina > 0.0f;
}

void UEmergeStaminaComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}
