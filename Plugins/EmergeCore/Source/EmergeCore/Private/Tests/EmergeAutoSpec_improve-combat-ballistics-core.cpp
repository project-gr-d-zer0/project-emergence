#include "Misc/AutomationTest.h"

#if WITH_AUTOMATION_TESTS

#include "UObject/Package.h"
#include "CombatBallistics.h"

// Feature under test: UCombatBallistics::ComputeRangeFalloff(InDistance, MinRange, MaxRange)
// Linear falloff factor across the effective range band:
//   factor = clamp((MaxRange - InDistance) / (MaxRange - MinRange), 0, 1)
// Boundary contracts:
//   InDistance == MinRange            -> exactly 1.0f
//   InDistance == MaxRange            -> exactly 0.0f
//   MaxRange <= MinRange (degenerate) -> exactly 1.0f (no division by zero / no NaN)

BEGIN_DEFINE_SPEC(FImproveCombatBallisticsCoreSpec, "Emergence.Combat.ImproveCombatBallisticsCore",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
	UCombatBallistics* Ballistics = nullptr;
END_DEFINE_SPEC(FImproveCombatBallisticsCoreSpec)

void FImproveCombatBallisticsCoreSpec::Define()
{
	BeforeEach([this]()
	{
		Ballistics = NewObject<UCombatBallistics>(GetTransientPackage());
	});

	Describe("ComputeRangeFalloff", [this]()
	{
		It("returns exactly 1.0f when InDistance equals MinRange", [this]()
		{
			TestEqual(TEXT("At MinRange"), Ballistics->ComputeRangeFalloff(100.0f, 100.0f, 500.0f), 1.0f);
		});

		It("returns exactly 0.0f when InDistance equals MaxRange", [this]()
		{
			TestEqual(TEXT("At MaxRange"), Ballistics->ComputeRangeFalloff(500.0f, 100.0f, 500.0f), 0.0f);
		});

		It("returns exactly 1.0f when MaxRange is less than MinRange", [this]()
		{
			TestEqual(TEXT("MaxRange < MinRange"), Ballistics->ComputeRangeFalloff(300.0f, 500.0f, 100.0f), 1.0f);
		});

		It("returns exactly 1.0f when MaxRange equals MinRange (no division error)", [this]()
		{
			const float Result = Ballistics->ComputeRangeFalloff(300.0f, 200.0f, 200.0f);
			TestFalse(TEXT("Result is finite (not NaN)"), FMath::IsNaN(Result));
			TestEqual(TEXT("MaxRange == MinRange"), Result, 1.0f);
		});
	});
}

#endif // WITH_AUTOMATION_TESTS