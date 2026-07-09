#include "Framework/EmergeGameMode.h"
#include "Framework/EmergePlayerController.h"
#include "NavigationSystem.h"
#include "TimerManager.h"
#include "Engine/World.h"

AEmergeGameMode::AEmergeGameMode()
{
	PrimaryActorTick.bCanEverTick = false;
	PlayerControllerClass = AEmergePlayerController::StaticClass();
}

void AEmergeGameMode::BeginPlay()
{
	Super::BeginPlay();
	// Dynamic navmesh doesn't reliably auto-generate tiles at PIE start here; force a build just after
	// play begins so AI pursuit + player NavigateTo work without a manual RebuildNavigation.
	FTimerHandle Th;
	GetWorldTimerManager().SetTimer(Th, this, &AEmergeGameMode::EnsureNavmesh, 0.5f, false);
}

void AEmergeGameMode::EnsureNavmesh()
{
	if (UNavigationSystemV1* Nav = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld()))
	{
		Nav->Build();
	}
}
