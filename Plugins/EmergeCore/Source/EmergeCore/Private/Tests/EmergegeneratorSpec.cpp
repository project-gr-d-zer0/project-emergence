// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: generator power (Project Zomboid).
// Closes the utility-timeline loop: after the grid cuts (EmergeUtilityGrid), a fuelled generator supplies
// local power, keeping fridges cold (EmergeFoodSpoilage). Fuel burns per hour proportional to the connected
// load. Numbers are exact from decompiled PZ B41 IsoGenerator.java: base draw totalPowerUsing = 0.02, each
// connected appliance adds 0.09 (large) / 0.03 (medium) / 0.01 (small); fuel burned per hour =
// totalPowerDraw * GeneratorFuelConsumption (sandbox default 1.0). Fuel is a 0..100 tank level.
// CONTRACT — NEW class UEmergeGenerator : public UBlueprintFunctionLibrary
//   in Public/Survival/EmergeGenerator.h + Private/Survival/EmergeGenerator.cpp, static UFUNCTIONs:
//
//   static float TotalPowerDraw(int32 NumLarge, int32 NumMedium, int32 NumSmall);
//     // return 0.02f + NumLarge * 0.09f + NumMedium * 0.03f + NumSmall * 0.01f;
//
//   static float FuelBurnPerHour(float TotalPowerDraw, float ConsumptionMult);
//     // return TotalPowerDraw * ConsumptionMult;
//
//   static float RuntimeHours(float Fuel, float FuelBurnPerHour);
//     // Hours until the tank empties. Guard divide-by-zero.
//     // if (FuelBurnPerHour <= 0.f) return TNumericLimits<float>::Max();
//     // return Fuel / FuelBurnPerHour;
//
//   static float FuelAfterHours(float Fuel, float FuelBurnPerHour, float Hours);
//     // return FMath::Max(0.f, Fuel - FuelBurnPerHour * Hours);
//
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Survival/EmergeGenerator.h"

BEGIN_DEFINE_SPEC(FEmergeGeneratorSpec, "Emergence.Survival.Generator",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeGeneratorSpec)
void FEmergeGeneratorSpec::Define()
{
	Describe("TotalPowerDraw", [this]()
	{
		It("base draw plus per-appliance load (exact PZ constants)", [this]()
		{
			TestEqual(TEXT("idle base"), UEmergeGenerator::TotalPowerDraw(0, 0, 0), 0.02f);
			TestEqual(TEXT("one large"), UEmergeGenerator::TotalPowerDraw(1, 0, 0), 0.11f);
			TestEqual(TEXT("one medium"), UEmergeGenerator::TotalPowerDraw(0, 1, 0), 0.05f);
			TestEqual(TEXT("one small"), UEmergeGenerator::TotalPowerDraw(0, 0, 1), 0.03f);
			TestEqual(TEXT("2 large 1 med 3 small"), UEmergeGenerator::TotalPowerDraw(2, 1, 3), 0.26f);
		});
	});

	Describe("FuelBurnPerHour", [this]()
	{
		It("scales draw by the sandbox consumption multiplier", [this]()
		{
			TestEqual(TEXT("default mult"), UEmergeGenerator::FuelBurnPerHour(0.11f, 1.0f), 0.11f);
			TestEqual(TEXT("double consumption"), UEmergeGenerator::FuelBurnPerHour(0.11f, 2.0f), 0.22f);
			TestEqual(TEXT("thrifty"), UEmergeGenerator::FuelBurnPerHour(0.02f, 0.5f), 0.01f);
		});
	});

	Describe("RuntimeHours", [this]()
	{
		It("tank level over burn rate, guarded against zero burn", [this]()
		{
			TestEqual(TEXT("1.0 fuel @ 0.02/hr -> 50h"), UEmergeGenerator::RuntimeHours(1.0f, 0.02f), 50.0f);
			TestEqual(TEXT("2.0 fuel @ 0.02/hr -> 100h"), UEmergeGenerator::RuntimeHours(2.0f, 0.02f), 100.0f);
			TestEqual(TEXT("0.5 fuel @ 0.01/hr -> 50h"), UEmergeGenerator::RuntimeHours(0.5f, 0.01f), 50.0f);
			TestTrue(TEXT("zero burn -> ~infinite"), UEmergeGenerator::RuntimeHours(10.0f, 0.0f) > 1.0e30f);
		});
	});

	Describe("FuelAfterHours", [this]()
	{
		It("drains linearly and clamps at empty", [this]()
		{
			TestEqual(TEXT("100 - 0.02*24"), UEmergeGenerator::FuelAfterHours(100.0f, 0.02f, 24.0f), 99.52f);
			TestEqual(TEXT("50 - 0.5*10 -> 45"), UEmergeGenerator::FuelAfterHours(50.0f, 0.5f, 10.0f), 45.0f);
			TestEqual(TEXT("overdraw clamps to 0"), UEmergeGenerator::FuelAfterHours(1.0f, 0.11f, 100.0f), 0.0f);
		});
	});
}
#endif
