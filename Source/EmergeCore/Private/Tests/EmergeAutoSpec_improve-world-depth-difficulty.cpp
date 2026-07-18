#if WITH_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "UObject/Package.h"
#include "WorldDepthDifficulty.h"

// Behavior under test: depth clamping of the difficulty multiplier.
// GetDifficultyMultiplier(Depth) evaluates a curve of Depth, but Depth is first
// clamped to [MinDepth, MaxDepth]. Beyond the bounds, the result equals exactly
// the curve value AT the bound (no extrapolation, no NaN, no throw).
// Config for this spec:
//   MinDepth = 100.0, MaxDepth = 1000.0
//   Curve: Multiplier(d) = 1.0 + (clamp(d, Min, Max) - Min) / (Max - Min)
//   => At Min: 1.0 ; At Max: 2.0 ; below Min caps to 1.0 ; above Max caps to 2.0.

BEGIN_DEFINE_SPEC(FImproveWorldDepthDifficultySpec, "Emergence.World.ImproveWorldDepthDifficulty",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
	UWorldDepthDifficulty* Subject;
END_DEFINE_SPEC(FImproveWorldDepthDifficultySpec)

void FImproveWorldDepthDifficultySpec::Define()
{
	BeforeEach([this]()
	{
		Subject = NewObject<UWorldDepthDifficulty>(GetTransientPackage());
		Subject->MinDepth = 100.0f;
		Subject->MaxDepth = 1000.0f;
	});

	It("caps to bound curve values beyond min/max without NaN", [this]()
	{
		// Below min -> capped to min value (1.0)
		const float BelowMin = Subject->GetDifficultyMultiplier(-500.0f);
		TestFalse(TEXT("BelowMin not NaN"), FMath::IsNaN(BelowMin));
		TestEqual(TEXT("BelowMin caps to min curve value"), BelowMin, 1.0f);

		// At min boundary
		TestEqual(TEXT("At min boundary"), Subject->GetDifficultyMultiplier(100.0f), 1.0f);

		// One-over min
		TestEqual(TEXT("One over min"), Subject->GetDifficultyMultiplier(101.0f), 1.0f + (1.0f / 900.0f));

		// At max boundary
		TestEqual(TEXT("At max boundary"), Subject->GetDifficultyMultiplier(1000.0f), 2.0f);

		// Above max -> capped to max value (2.0)
		const float AboveMax = Subject->GetDifficultyMultiplier(9999.0f);
		TestFalse(TEXT("AboveMax not NaN"), FMath::IsNaN(AboveMax));
		TestEqual(TEXT("AboveMax caps to max curve value"), AboveMax, 2.0f);

		// Zero boundary (below min) also caps
		TestEqual(TEXT("Zero caps to min curve value"), Subject->GetDifficultyMultiplier(0.0f), 1.0f);
	});
}

#endif // WITH_AUTOMATION_TESTS