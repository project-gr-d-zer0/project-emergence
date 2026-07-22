// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: fire spread risk (survival).
// An open fire can spread to nearby flammables; chance depends on weather (rain suppresses, clear/wind feeds it)
// and dry fuel. The roll is passed in (deterministic). Composes with EmergeCampfire / EmergeWeatherCold.
// TUNABLE design chances, fixed model.
// CONTRACT — NEW class UEmergeFireSpread : public UBlueprintFunctionLibrary
//   in Public/Survival/EmergeFireSpread.h + Private/Survival/EmergeFireSpread.cpp, static UFUNCTIONs.
//   Weather int32: 0 Clear, 1 Rain, 2 Snow, 3 Storm.
//
//   static float SpreadChance(int32 Weather, bool bDryFuel);
//     // float c; switch(Weather){ case 0: c=0.2f; break; case 1: c=0.05f; break; case 2: c=0.02f; break; case 3: c=0.1f; break; default: c=0.f; }
//     // if (bDryFuel) c *= 2.0f;   return FMath::Clamp(c, 0.0f, 1.0f);
//   static bool Spreads(float Roll01, float Chance);   // return Roll01 < Chance;
//
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Survival/EmergeFireSpread.h"

BEGIN_DEFINE_SPEC(FEmergeFireSpreadSpec, "Emergence.Survival.FireSpread",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeFireSpreadSpec)
void FEmergeFireSpreadSpec::Define()
{
	Describe("SpreadChance", [this]()
	{
		It("weather and dry fuel drive spread", [this]()
		{
			TestEqual(TEXT("clear -> 0.2"), UEmergeFireSpread::SpreadChance(0, false), 0.2f);
			TestEqual(TEXT("clear + dry -> 0.4"), UEmergeFireSpread::SpreadChance(0, true), 0.4f);
			TestEqual(TEXT("rain -> 0.05"), UEmergeFireSpread::SpreadChance(1, false), 0.05f);
			TestEqual(TEXT("rain + dry -> 0.1"), UEmergeFireSpread::SpreadChance(1, true), 0.1f);
			TestEqual(TEXT("snow -> 0.02"), UEmergeFireSpread::SpreadChance(2, false), 0.02f);
		});
	});

	Describe("Spreads", [this]()
	{
		It("spreads when roll under chance", [this]()
		{
			TestTrue(TEXT("0.1 under 0.2"), UEmergeFireSpread::Spreads(0.1f, 0.2f));
			TestFalse(TEXT("0.3 over 0.2"), UEmergeFireSpread::Spreads(0.3f, 0.2f));
		});
	});
}
#endif
