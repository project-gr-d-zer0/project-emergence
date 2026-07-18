#if WITH_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "CraftingRecipe.h"

// FORMULA (exact, deterministic — no RNG, no wall-clock):
//   CompletionTime = BaseTime * TierModifier / WorkerCount
//   - BaseTime:      recipe base duration in seconds (float)
//   - TierModifier:  hideout tier multiplier (e.g. tier 1 => 1.0, faster tiers < 1.0)
//   - WorkerCount:   number of assigned workers (>= 1); clamped to minimum 1
// Boundaries covered: WorkerCount 0 (clamped to 1), WorkerCount 1 (one-over baseline),
//   BaseTime 0 (=> 0.0), high worker count, non-trivial tier modifier.

BEGIN_DEFINE_SPEC(FCraftingQueueTimingSpec, "Emergence.Crafting.QueueTiming",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FCraftingQueueTimingSpec)

void FCraftingQueueTimingSpec::Define()
{
	Describe("CalculateCompletionTime", [this]()
	{
		It("returns base * tier / workers for a standard case", [this]()
		{
			UCraftingRecipe* Recipe = NewObject<UCraftingRecipe>(GetTransientPackage());
			// 60 * 0.5 / 3 = 10.0
			TestEqual(TEXT("standard"), Recipe->CalculateCompletionTime(60.0f, 0.5f, 3), 10.0f);
		});

		It("returns base * tier at one worker (one-over baseline)", [this]()
		{
			UCraftingRecipe* Recipe = NewObject<UCraftingRecipe>(GetTransientPackage());
			// 60 * 1.0 / 1 = 60.0
			TestEqual(TEXT("one worker"), Recipe->CalculateCompletionTime(60.0f, 1.0f, 1), 60.0f);
		});

		It("clamps worker count of 0 to 1", [this]()
		{
			UCraftingRecipe* Recipe = NewObject<UCraftingRecipe>(GetTransientPackage());
			// 60 * 1.0 / max(1,0) = 60.0
			TestEqual(TEXT("zero workers clamped"), Recipe->CalculateCompletionTime(60.0f, 1.0f, 0), 60.0f);
		});

		It("returns 0 when base time is 0", [this]()
		{
			UCraftingRecipe* Recipe = NewObject<UCraftingRecipe>(GetTransientPackage());
			TestEqual(TEXT("zero base"), Recipe->CalculateCompletionTime(0.0f, 0.5f, 4), 0.0f);
		});

		It("handles a high worker count deterministically", [this]()
		{
			UCraftingRecipe* Recipe = NewObject<UCraftingRecipe>(GetTransientPackage());
			// 100 * 0.8 / 8 = 10.0
			TestEqual(TEXT("high workers"), Recipe->CalculateCompletionTime(100.0f, 0.8f, 8), 10.0f);
		});
	});
}

#endif // WITH_AUTOMATION_TESTS