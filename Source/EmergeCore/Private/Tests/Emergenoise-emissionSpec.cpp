// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: noise emission per action (survival/AI).
// How far a player's action can be heard - the input to AI hearing / zombie attraction. Sneaking is near-silent,
// sprinting loud, a gunshot carries far. TUNABLE design radii (metres), fixed deterministic model; composes with
// the existing EmergeNoise AI plumbing.
// CONTRACT — NEW class UEmergeNoiseEmission : public UBlueprintFunctionLibrary
//   in Public/Survival/EmergeNoiseEmission.h + Private/Survival/EmergeNoiseEmission.cpp, static UFUNCTIONs.
//   Action int32: 0 Sneak, 1 Walk, 2 Run, 3 Sprint, 4 Gunshot.
//
//   static float NoiseRadius(int32 Action);
//     // switch: 0 -> 2.f; 1 -> 5.f; 2 -> 15.f; 3 -> 25.f; 4 -> 100.f; default -> 0.f;
//   static bool  CanHear(float NoiseRadius, float ListenerDistance);   // return ListenerDistance <= NoiseRadius;
//
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Survival/EmergeNoiseEmission.h"

BEGIN_DEFINE_SPEC(FEmergeNoiseEmissionSpec, "Emergence.Survival.NoiseEmission",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeNoiseEmissionSpec)
void FEmergeNoiseEmissionSpec::Define()
{
	Describe("NoiseRadius", [this]()
	{
		It("louder actions carry farther (exact radii)", [this]()
		{
			TestEqual(TEXT("sneak 2m"), UEmergeNoiseEmission::NoiseRadius(0), 2.0f);
			TestEqual(TEXT("walk 5m"), UEmergeNoiseEmission::NoiseRadius(1), 5.0f);
			TestEqual(TEXT("run 15m"), UEmergeNoiseEmission::NoiseRadius(2), 15.0f);
			TestEqual(TEXT("sprint 25m"), UEmergeNoiseEmission::NoiseRadius(3), 25.0f);
			TestEqual(TEXT("gunshot 100m"), UEmergeNoiseEmission::NoiseRadius(4), 100.0f);
			TestEqual(TEXT("idle/unknown 0"), UEmergeNoiseEmission::NoiseRadius(9), 0.0f);
		});
	});

	Describe("CanHear", [this]()
	{
		It("a listener within the radius hears it", [this]()
		{
			TestTrue(TEXT("10m inside a 15m run"), UEmergeNoiseEmission::CanHear(15.f, 10.f));
			TestTrue(TEXT("at the edge"), UEmergeNoiseEmission::CanHear(15.f, 15.f));
			TestFalse(TEXT("20m outside a 15m run"), UEmergeNoiseEmission::CanHear(15.f, 20.f));
		});
	});
}
#endif
