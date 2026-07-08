// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: faction reputation/standing.
// Reputation (-100..100) changes with actions; standing tiers gate access (e.g. safe-zone groups, traders).
// Ties factions <-> economy <-> safe zones.
// CONTRACT — NEW class UEmergeReputation : public UBlueprintFunctionLibrary
//   in Public/Economy/EmergeReputation.h + Private/Economy/EmergeReputation.cpp, static UFUNCTIONs:
//   static int32 ApplyRepChange(int32 Current, int32 Delta);  // return FMath::Clamp(Current + Delta, -100, 100);
//   static int32 StandingTier(int32 Rep);
//     // Rep < -50 -> 0 (Hostile); -50..49 -> 1 (Neutral); 50..89 -> 2 (Friendly); >=90 -> 3 (Allied)
//   static bool  CanAccessSafeZone(int32 Rep);   // return Rep >= -50;  (hostiles are turned away)
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Economy/EmergeReputation.h"

BEGIN_DEFINE_SPEC(FEmergeReputationSpec, "Emergence.Economy.Reputation",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeReputationSpec)
void FEmergeReputationSpec::Define()
{
	Describe("ApplyRepChange (clamped -100..100)", [this]()
	{
		It("matches exact golden values and clamps", [this]()
		{
			TestEqual(TEXT("0 + 20 -> 20"),        UEmergeReputation::ApplyRepChange(0, 20), 20);
			TestEqual(TEXT("90 + 20 -> 100 (cap)"), UEmergeReputation::ApplyRepChange(90, 20), 100);
			TestEqual(TEXT("-90 - 20 -> -100 (floor)"), UEmergeReputation::ApplyRepChange(-90, -20), -100);
		});
	});

	Describe("StandingTier (hostile/neutral/friendly/allied)", [this]()
	{
		It("maps each band and boundary", [this]()
		{
			TestEqual(TEXT("-60 -> Hostile 0"),   UEmergeReputation::StandingTier(-60), 0);
			TestEqual(TEXT("-50 -> Neutral 1"),   UEmergeReputation::StandingTier(-50), 1);
			TestEqual(TEXT("49 -> Neutral 1"),    UEmergeReputation::StandingTier(49), 1);
			TestEqual(TEXT("50 -> Friendly 2"),   UEmergeReputation::StandingTier(50), 2);
			TestEqual(TEXT("90 -> Allied 3"),     UEmergeReputation::StandingTier(90), 3);
		});
	});

	Describe("CanAccessSafeZone (not hostile)", [this]()
	{
		It("turns away hostiles only", [this]()
		{
			TestTrue(TEXT("neutral 0 allowed"),   UEmergeReputation::CanAccessSafeZone(0));
			TestTrue(TEXT("-50 boundary allowed"),UEmergeReputation::CanAccessSafeZone(-50));
			TestFalse(TEXT("-51 hostile denied"), UEmergeReputation::CanAccessSafeZone(-51));
		});
	});
}
#endif
