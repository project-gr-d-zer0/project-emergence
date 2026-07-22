#include "Misc/AutomationTest.h"
#include "BallisticAccuracyResolver.h"

#if WITH_AUTOMATION_TESTS

// FORMULA (exact math the implementer must realize):
//   Given BaseAccuracy in [0,100], EncumbranceTier in {0=Unburdened,1=Light,2=Heavy,3=Overloaded},
//   and bool bBleeding, bool bStunned as vital status effects.
//
//   EncumbranceMultiplier follows a non-linear (quadratic) decay:
//     encM = 1 - 0.09 * (tier^2)      // tier 0->1.00, 1->0.91, 2->0.64, 3->0.19
//
//   VitalMultiplier is multiplicative:
//     vitM = (bBleeding ? 0.85 : 1.0) * (bStunned ? 0.50 : 1.0)
//
//   FinalHitChance = clamp(BaseAccuracy * encM * vitM, 0, 100), rounded to 2 decimals.
//
// Resolver signature under test:
//   float UBallisticAccuracyResolver::ResolveHitChance(float BaseAccuracy, int32 EncumbranceTier, bool bBleeding, bool bStunned)

BEGIN_DEFINE_SPEC(FBallisticAccuracyModifierSpec, "Emergence.Inventory.BallisticAccuracyModifier",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
	UBallisticAccuracyResolver* Resolver;
END_DEFINE_SPEC(FBallisticAccuracyModifierSpec)

void FBallisticAccuracyModifierSpec::Define()
{
	BeforeEach([this]() { Resolver = NewObject<UBallisticAccuracyResolver>(GetTransientPackage()); });

	Describe("ResolveHitChance", [this]()
	{
		It("computes non-linear encumbrance decay with no vital effects", [this]()
		{
			TestEqual(TEXT("Tier0 unburdened"), Resolver->ResolveHitChance(80.0f, 0, false, false), 80.0f);
			TestEqual(TEXT("Tier1 light"), Resolver->ResolveHitChance(80.0f, 1, false, false), 72.8f);
			TestEqual(TEXT("Tier2 heavy"), Resolver->ResolveHitChance(80.0f, 2, false, false), 51.2f);
			TestEqual(TEXT("Tier3 overloaded"), Resolver->ResolveHitChance(80.0f, 3, false, false), 15.2f);
		});

		It("applies multiplicative vital status modifiers", [this]()
		{
			TestEqual(TEXT("Bleeding only"), Resolver->ResolveHitChance(80.0f, 0, true, false), 68.0f);
			TestEqual(TEXT("Stunned only"), Resolver->ResolveHitChance(80.0f, 0, false, true), 40.0f);
			TestEqual(TEXT("Bleeding+Stunned+Heavy"), Resolver->ResolveHitChance(80.0f, 2, true, true), 21.76f);
		});

		It("covers boundaries (0, max, one-over-max)", [this]()
		{
			TestEqual(TEXT("Zero base"), Resolver->ResolveHitChance(0.0f, 0, false, false), 0.0f);
			TestEqual(TEXT("Max base"), Resolver->ResolveHitChance(100.0f, 0, false, false), 100.0f);
			TestEqual(TEXT("Over-max clamps to 100"), Resolver->ResolveHitChance(150.0f, 0, false, false), 100.0f);
			TestEqual(TEXT("Negative clamps to 0"), Resolver->ResolveHitChance(-5.0f, 0, false, false), 0.0f);
		});
	});
}

#endif // WITH_AUTOMATION_TESTS