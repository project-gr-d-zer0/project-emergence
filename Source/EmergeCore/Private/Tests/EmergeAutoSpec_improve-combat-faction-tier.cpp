#include "Misc/AutomationTest.h"

#if WITH_AUTOMATION_TESTS

#include "UObject/Package.h"
#include "CombatFactionTier.h"

// FORMULA (feature to implement on UCombatFactionTier):
//   float ApplyTierMultiplier(float Base, float Multiplier)
//   - MaxMultiplier bound is configurable via SetMaxMultiplier(float); default = 4.0f
//   - If Multiplier is NaN/Inf OR out-of-range (Multiplier < 0 OR Multiplier > MaxMultiplier),
//     the result is capped to exactly the Base value (invalid input => no scaling).
//   - Otherwise result = Base * Multiplier.
//   - Result must never be NaN/Inf and must never exceed Base * MaxMultiplier.
// This spec pins Base = 1.0f so an invalid/out-of-range multiplier yields exactly 1.0f.

BEGIN_DEFINE_SPEC(FImproveCombatFactionTierSpec, "Emergence.Combat.ImproveCombatFactionTier",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
	UCombatFactionTier* Tier;
END_DEFINE_SPEC(FImproveCombatFactionTierSpec)

void FImproveCombatFactionTierSpec::Define()
{
	BeforeEach([this]()
	{
		Tier = NewObject<UCombatFactionTier>(GetTransientPackage());
		Tier->SetMaxMultiplier(4.0f);
	});

	Describe("ApplyTierMultiplier with invalid/out-of-range input", [this]()
	{
		It("caps NaN multiplier to base 1.0 without NaN propagation", [this]()
		{
			const float Result = Tier->ApplyTierMultiplier(1.0f, FMath::Sqrt(-1.0f));
			TestFalse(TEXT("Result is not NaN"), FMath::IsNaN(Result));
			TestEqual(TEXT("NaN multiplier => base 1.0"), Result, 1.0f);
		});

		It("caps negative (below 0 boundary) multiplier to base 1.0", [this]()
		{
			TestEqual(TEXT("multiplier -1 => base 1.0"), Tier->ApplyTierMultiplier(1.0f, -1.0f), 1.0f);
		});

		It("accepts exactly 0 boundary as valid scaling", [this]()
		{
			TestEqual(TEXT("multiplier 0 => 0.0"), Tier->ApplyTierMultiplier(1.0f, 0.0f), 0.0f);
		});

		It("accepts exactly max boundary as valid scaling", [this]()
		{
			TestEqual(TEXT("multiplier 4 (max) => 4.0"), Tier->ApplyTierMultiplier(1.0f, 4.0f), 4.0f);
		});

		It("caps one-over-max (4.0001) multiplier to base 1.0 and never exceeds max bound", [this]()
		{
			const float Result = Tier->ApplyTierMultiplier(1.0f, 4.0001f);
			TestEqual(TEXT("over-max multiplier => base 1.0"), Result, 1.0f);
			TestTrue(TEXT("Result never exceeds Base * MaxMultiplier"), Result <= 4.0f);
		});
	});
}

#endif // WITH_AUTOMATION_TESTS