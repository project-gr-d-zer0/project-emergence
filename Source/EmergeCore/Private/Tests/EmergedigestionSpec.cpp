// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: digestion / stomach (DayZ-style).
// The INPUT side that refuels EmergeMetabolism: eaten food sits in the stomach and is digested each tick,
// transferring energy (kcal) and water (ml) into the vitals that EmergeMetabolism drains and EmergeDisease's
// immunity reads; contaminated food also passes transmission agents in proportion to its digestibility
// (the EmergeDisease/EmergePoisoning source). Numbers exact from decompiled DayZ playerconstants.c: DIGESTION_SPEED
// 1.7/s, STOMACH_ENERGY_TRANSFERED_PER_SEC 3, STOMACH_WATER_TRANSFERED_PER_SEC 6, STOMACH_SOLID_EMPTIED_PER_SEC 7,
// fullness volume levels 1 / 750 / 1000; agentbase.c agents_transfered = digested_amount * m_Digestibility.
// CONTRACT — NEW class UEmergeDigestion : public UBlueprintFunctionLibrary
//   in Public/Survival/EmergeDigestion.h + Private/Survival/EmergeDigestion.cpp, static UFUNCTIONs.
//
//   static float DigestionPerTick(float DeltaT);       // return 1.7f * DeltaT;  // DIGESTION_SPEED
//   static float SolidEmptiedPerTick(float DeltaT);    // return 7.0f * DeltaT;  // STOMACH_SOLID_EMPTIED_PER_SEC
//   static float EnergyTransferPerTick(float DeltaT);  // return 3.0f * DeltaT;  // STOMACH_ENERGY_TRANSFERED_PER_SEC
//   static float WaterTransferPerTick(float DeltaT);   // return 6.0f * DeltaT;  // STOMACH_WATER_TRANSFERED_PER_SEC
//
//   static int32 FullnessLevel(float StomachVolume);
//     // if (StomachVolume >= 1000.0f) return 3;  if (StomachVolume >= 750.0f) return 2;
//     // if (StomachVolume >= 1.0f) return 1;      return 0;   // BT_STOMACH_VOLUME_LVL3/2/1
//
//   static float AgentsTransferred(float DigestedAmount, float Digestibility);
//     // return DigestedAmount * Digestibility;   // agentbase.c: agents in from an infected item
//
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Survival/EmergeDigestion.h"

BEGIN_DEFINE_SPEC(FEmergeDigestionSpec, "Emergence.Survival.Digestion",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeDigestionSpec)
void FEmergeDigestionSpec::Define()
{
	Describe("Per-tick rates", [this]()
	{
		It("digestion/solid/energy/water transfer scale linearly with dt (exact DayZ constants)", [this]()
		{
			TestEqual(TEXT("digestion dt=1 -> 1.7"), UEmergeDigestion::DigestionPerTick(1.0f), 1.7f);
			TestEqual(TEXT("digestion dt=2 -> 3.4"), UEmergeDigestion::DigestionPerTick(2.0f), 3.4f);
			TestEqual(TEXT("solid empty dt=1 -> 7"), UEmergeDigestion::SolidEmptiedPerTick(1.0f), 7.0f);
			TestEqual(TEXT("solid empty dt=2 -> 14"), UEmergeDigestion::SolidEmptiedPerTick(2.0f), 14.0f);
			TestEqual(TEXT("energy dt=1 -> 3"), UEmergeDigestion::EnergyTransferPerTick(1.0f), 3.0f);
			TestEqual(TEXT("energy dt=10 -> 30"), UEmergeDigestion::EnergyTransferPerTick(10.0f), 30.0f);
			TestEqual(TEXT("water dt=1 -> 6"), UEmergeDigestion::WaterTransferPerTick(1.0f), 6.0f);
			TestEqual(TEXT("water dt=0.5 -> 3"), UEmergeDigestion::WaterTransferPerTick(0.5f), 3.0f);
		});
	});

	Describe("FullnessLevel", [this]()
	{
		It("stomach volume -> badge level at exact thresholds", [this]()
		{
			TestEqual(TEXT("empty 0 -> 0"), UEmergeDigestion::FullnessLevel(0.0f), 0);
			TestEqual(TEXT("1 -> level 1"), UEmergeDigestion::FullnessLevel(1.0f), 1);
			TestEqual(TEXT("749 -> level 1"), UEmergeDigestion::FullnessLevel(749.0f), 1);
			TestEqual(TEXT("750 -> level 2"), UEmergeDigestion::FullnessLevel(750.0f), 2);
			TestEqual(TEXT("999 -> level 2"), UEmergeDigestion::FullnessLevel(999.0f), 2);
			TestEqual(TEXT("1000 -> level 3"), UEmergeDigestion::FullnessLevel(1000.0f), 3);
			TestEqual(TEXT("1500 -> level 3"), UEmergeDigestion::FullnessLevel(1500.0f), 3);
		});
	});

	Describe("AgentsTransferred", [this]()
	{
		It("infected food passes agents = digested amount * digestibility", [this]()
		{
			TestEqual(TEXT("100g * 0.5 -> 50"), UEmergeDigestion::AgentsTransferred(100.0f, 0.5f), 50.0f);
			TestEqual(TEXT("200 * 1.0 -> 200"), UEmergeDigestion::AgentsTransferred(200.0f, 1.0f), 200.0f);
			TestEqual(TEXT("clean food 0 digestibility -> 0"), UEmergeDigestion::AgentsTransferred(300.0f, 0.0f), 0.0f);
			TestEqual(TEXT("nothing digested -> 0"), UEmergeDigestion::AgentsTransferred(0.0f, 0.5f), 0.0f);
		});
	});
}
#endif
