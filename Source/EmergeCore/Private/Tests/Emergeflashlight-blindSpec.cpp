// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: flashlight/laser blind (combat).
// Shining a light in an enemy's eyes at close range while they face you blinds them; effect drops with distance
// and is zero if they aren't looking toward the source. Composes with EmergeSuppression-style aim penalties.
// TUNABLE model, fixed deterministic.
// CONTRACT — NEW class UEmergeFlashlightBlind : public UBlueprintFunctionLibrary
//   in Public/Combat/EmergeFlashlightBlind.h + Private/Combat/EmergeFlashlightBlind.cpp, static UFUNCTIONs.
//
//   static float BlindIntensity(float Distance, float MaxRange, bool bFacing);
//     // if (!bFacing) return 0.0f;   if (Distance >= MaxRange) return 0.0f;   return 1.0f - Distance / MaxRange;
//   static bool  IsBlinded(float Intensity, float Threshold);   // return Intensity >= Threshold;
//
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Combat/EmergeFlashlightBlind.h"

BEGIN_DEFINE_SPEC(FEmergeFlashlightBlindSpec, "Emergence.Combat.FlashlightBlind",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeFlashlightBlindSpec)
void FEmergeFlashlightBlindSpec::Define()
{
	Describe("BlindIntensity", [this]()
	{
		It("strong up close and facing, none if turned away or far", [this]()
		{
			TestEqual(TEXT("point blank facing -> 1"), UEmergeFlashlightBlind::BlindIntensity(0.f, 10.f, true), 1.0f);
			TestEqual(TEXT("half range facing -> 0.5"), UEmergeFlashlightBlind::BlindIntensity(5.f, 10.f, true), 0.5f);
			TestEqual(TEXT("at range -> 0"), UEmergeFlashlightBlind::BlindIntensity(10.f, 10.f, true), 0.0f);
			TestEqual(TEXT("not facing -> 0"), UEmergeFlashlightBlind::BlindIntensity(5.f, 10.f, false), 0.0f);
		});
	});

	Describe("IsBlinded", [this]()
	{
		It("blinded past the threshold", [this]()
		{
			TestTrue(TEXT("0.5 over 0.3"), UEmergeFlashlightBlind::IsBlinded(0.5f, 0.3f));
			TestFalse(TEXT("0.2 under 0.3"), UEmergeFlashlightBlind::IsBlinded(0.2f, 0.3f));
		});
	});
}
#endif
