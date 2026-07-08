// HUMAN-AUTHORED trusted acceptance spec. Feature: runtime stagger component wraps the tested kernel.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Combat/EmergeStaggerComponent.h"

BEGIN_DEFINE_SPEC(FEmergeStaggerComponentSpec, "Emergence.Combat.StaggerComponent",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeStaggerComponentSpec)
void FEmergeStaggerComponentSpec::Define()
{
	Describe("ApplyHit accumulates and Recover decays the meter", [this]()
	{
		It("builds meter from a hit, exposes state/speed, and recovers to moving", [this]()
		{
			UEmergeStaggerComponent* C = NewObject<UEmergeStaggerComponent>();
			// Torso(1.0) x Front(1.0): +100 -> Stagger band [80,120), speed 0
			C->ApplyHit(100.0f, EEmergeBodyPart::Torso, EEmergeHitDir::Front);
			TestEqual(TEXT("meter = 100"), C->Meter, 100.0f);
			TestEqual(TEXT("state = Stagger"), (int32)C->CurrentState(), (int32)EEmergeStaggerState::Stagger);
			TestEqual(TEXT("speed 0 while staggered"), C->SpeedMultiplier(), 0.0f);

			C->RecoveryPerSec = 20.0f;
			C->Recover(4.0f);                                   // 100 - 20*4 = 20 -> Moving band (<40)
			TestEqual(TEXT("meter 100-80 -> 20"), C->Meter, 20.0f);
			TestEqual(TEXT("state = Moving"), (int32)C->CurrentState(), (int32)EEmergeStaggerState::Moving);
			TestEqual(TEXT("full speed recovered"), C->SpeedMultiplier(), 1.0f);
		});
	});
}
#endif
