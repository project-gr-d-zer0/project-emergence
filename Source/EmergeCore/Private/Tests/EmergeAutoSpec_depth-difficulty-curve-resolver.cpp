// Emergence.World.DepthDifficultyCurveResolver
//
// Behavior under test: The resolver samples EmergeWorldDifficultyConfig at a given
// depth against a configured max difficulty and returns three curve outputs:
//   HordeDensityMultiplier, FactionTierIndex, LootWeightModifier.
//
// Sampling contract (deterministic, no RNG, no time):
//   alpha = clamp(depth / maxDifficulty, 0.0, 1.0)        // depth 3.5, max 10 -> alpha = 0.35
//   HordeDensityMultiplier = 1.0 + alpha * 3.0            // 1.0 + 0.35*3.0 = 2.05
//   FactionTierIndex       = floor(alpha * (NumTiers-1))  // NumTiers=5 -> floor(0.35*4)=floor(1.4)=1
//   LootWeightModifier     = 0.5 + alpha * 1.5            // 0.5 + 0.35*1.5 = 1.025
// All outputs must be finite (no NaN) at every sampled point.

#if WITH_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "UObject/Package.h"
#include "World/EmergeWorldDifficultyConfig.h"
#include "World/EmergeDepthDifficultyCurveResolver.h"

BEGIN_DEFINE_SPEC(FDepthDifficultyCurveResolverSpec,
	"Emergence.World.DepthDifficultyCurveResolver",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
	UEmergeWorldDifficultyConfig* Config = nullptr;
	UEmergeDepthDifficultyCurveResolver* Resolver = nullptr;
END_DEFINE_SPEC(FDepthDifficultyCurveResolverSpec)

void FDepthDifficultyCurveResolverSpec::Define()
{
	BeforeEach([this]()
	{
		Config = NewObject<UEmergeWorldDifficultyConfig>(GetTransientPackage());
		Config->MaxDifficulty = 10.0f;
		Config->NumFactionTiers = 5;
		Resolver = NewObject<UEmergeDepthDifficultyCurveResolver>(GetTransientPackage());
		Resolver->Configure(Config);
	});

	It("returns exact golden values at depth 3.5", [this]()
	{
		FEmergeDifficultySample S = Resolver->ResolveAtDepth(3.5f);
		TestFalse(TEXT("horde not NaN"), FMath::IsNaN(S.HordeDensityMultiplier));
		TestFalse(TEXT("loot not NaN"), FMath::IsNaN(S.LootWeightModifier));
		TestEqual(TEXT("horde density"), S.HordeDensityMultiplier, 2.05f, 1e-4f);
		TestEqual(TEXT("faction tier"), S.FactionTierIndex, 1);
		TestEqual(TEXT("loot weight"), S.LootWeightModifier, 1.025f, 1e-4f);
	});

	It("clamps at boundaries 0, max, and one-over-max", [this]()
	{
		FEmergeDifficultySample Zero = Resolver->ResolveAtDepth(0.0f);
		TestEqual(TEXT("horde@0"), Zero.HordeDensityMultiplier, 1.0f, 1e-4f);
		TestEqual(TEXT("tier@0"), Zero.FactionTierIndex, 0);
		TestEqual(TEXT("loot@0"), Zero.LootWeightModifier, 0.5f, 1e-4f);

		FEmergeDifficultySample Max = Resolver->ResolveAtDepth(10.0f);
		TestEqual(TEXT("horde@max"), Max.HordeDensityMultiplier, 4.0f, 1e-4f);
		TestEqual(TEXT("tier@max"), Max.FactionTierIndex, 4);
		TestEqual(TEXT("loot@max"), Max.LootWeightModifier, 2.0f, 1e-4f);

		FEmergeDifficultySample Over = Resolver->ResolveAtDepth(11.0f);
		TestEqual(TEXT("horde@over clamps to max"), Over.HordeDensityMultiplier, 4.0f, 1e-4f);
		TestEqual(TEXT("tier@over clamps to max"), Over.FactionTierIndex, 4);
		TestEqual(TEXT("loot@over clamps to max"), Over.LootWeightModifier, 2.0f, 1e-4f);
	});
}

#endif // WITH_AUTOMATION_TESTS