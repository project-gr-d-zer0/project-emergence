// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: compass degrees/mils readout (shell).
// SHELL stratum: the numeric compass readout - a normalized integer bearing and the same bearing in mils (for
// artillery/range calls). Pairs with EmergeCompassHud / EmergeCompassBearing. Standard bearing math; deterministic.
// NOTE: mils use a 0.5-mil tolerance (6400/360 is not float-exact, so large bearings accumulate rounding).
// CONTRACT — NEW class UEmergeCompassText : public UBlueprintFunctionLibrary
//   in Public/Shell/EmergeCompassText.h + Private/Shell/EmergeCompassText.cpp, static UFUNCTIONs.
//
//   static int32 DegreesInt(float HeadingDeg);
//     // float d = FMath::Fmod(HeadingDeg, 360.0f); if (d < 0.0f) d += 360.0f;
//     // return FMath::RoundToInt(d) % 360;
//   static float ToMils(float HeadingDeg);   // return HeadingDeg * (6400.0f / 360.0f);   // NATO mils
//
// Deterministic, exact golden values (mils within 0.5), boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Shell/EmergeCompassText.h"

BEGIN_DEFINE_SPEC(FEmergeCompassTextSpec, "Emergence.Shell.CompassText",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeCompassTextSpec)
void FEmergeCompassTextSpec::Define()
{
	Describe("DegreesInt", [this]()
	{
		It("normalized integer bearing", [this]()
		{
			TestEqual(TEXT("45.4 -> 45"), UEmergeCompassText::DegreesInt(45.4f), 45);
			TestEqual(TEXT("45.6 -> 46"), UEmergeCompassText::DegreesInt(45.6f), 46);
			TestEqual(TEXT("360 wraps -> 0"), UEmergeCompassText::DegreesInt(360.f), 0);
			TestEqual(TEXT("-90 -> 270"), UEmergeCompassText::DegreesInt(-90.f), 270);
		});
	});

	Describe("ToMils", [this]()
	{
		It("NATO mils (0.5-mil tolerance for float)", [this]()
		{
			TestEqual(TEXT("0deg -> 0 mils"), UEmergeCompassText::ToMils(0.f), 0.0f, 0.5f);
			TestEqual(TEXT("90deg -> 1600 mils"), UEmergeCompassText::ToMils(90.f), 1600.0f, 0.5f);
			TestEqual(TEXT("360deg -> 6400 mils"), UEmergeCompassText::ToMils(360.f), 6400.0f, 0.5f);
		});
	});
}
#endif
