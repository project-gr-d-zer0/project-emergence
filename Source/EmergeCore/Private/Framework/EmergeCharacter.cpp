#include "Framework/EmergeCharacter.h"
#include "Survival/EmergeVitalsComponent.h"
#include "Combat/EmergeStaggerComponent.h"

AEmergeCharacter::AEmergeCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	Vitals = CreateDefaultSubobject<UEmergeVitalsComponent>(TEXT("Vitals"));
	Stagger = CreateDefaultSubobject<UEmergeStaggerComponent>(TEXT("Stagger"));
}
