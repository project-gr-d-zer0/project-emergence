// HUMAN-AUTHORED trusted acceptance spec. Feature: minimal gameplay framework classes,
// so the first playable map just assigns these + drops the character in a level.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Framework/EmergeGameMode.h"
#include "Framework/EmergeCharacter.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/Character.h"

BEGIN_DEFINE_SPEC(FEmergeFrameworkSpec, "Emergence.Framework.Classes",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeFrameworkSpec)
void FEmergeFrameworkSpec::Define()
{
	Describe("Gameplay framework classes", [this]()
	{
		It("exist and derive from the correct engine base classes", [this]()
		{
			TestNotNull(TEXT("AEmergeGameMode class exists"), AEmergeGameMode::StaticClass());
			TestNotNull(TEXT("AEmergeCharacter class exists"), AEmergeCharacter::StaticClass());
			TestTrue(TEXT("GameMode derives from AGameModeBase"),
				AEmergeGameMode::StaticClass()->IsChildOf(AGameModeBase::StaticClass()));
			TestTrue(TEXT("Character derives from ACharacter"),
				AEmergeCharacter::StaticClass()->IsChildOf(ACharacter::StaticClass()));
		});
	});
}
#endif
