// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: food poisoning / toxicity (DayZ-style).
// The consequence side of EmergeDisease: a FOOD_POISON agent load makes the player vomit, which drains water
// and energy (feeding back into EmergeMetabolism + EmergeDisease::Immunity01) and burns off agents; toxicity
// (heavy-metal/chemical) decays steadily and forces a vomit above its threshold. Numbers exact from decompiled
// DayZ toxicity.c (TOXICITY_CLEANUP_PER_SEC 1, VOMIT_THRESHOLD 70) + poisoning.c (activate agent>=1, vomit only
// above the 40 deactivate floor, occurrence Lerp 2..5 over agent/max then /300, water drain 70, energy drain 55,
// agent reduce 60 per vomit) + foodpoisonagent.c (m_MaxCount 800). The vomit ROLL itself is RNG and is NOT
// asserted here; every rate/threshold/derived value is deterministic.
// CONTRACT — NEW class UEmergePoisoning : public UBlueprintFunctionLibrary
//   in Public/Survival/EmergePoisoning.h + Private/Survival/EmergePoisoning.cpp, static UFUNCTIONs.
//
//   static float ToxicityDecayPerTick(float DeltaT);      // return -1.0f * DeltaT;   // TOXICITY_CLEANUP_PER_SEC
//   static bool  ShouldVomitFromToxicity(float Toxicity); // return Toxicity > 70.0f; // VOMIT_THRESHOLD
//   static bool  PoisoningActive(int32 FoodPoisonAgentCount); // return FoodPoisonAgentCount >= 1; // ACTIVATE
//   static bool  VomitingActive(int32 FoodPoisonAgentCount);  // return FoodPoisonAgentCount > 40; // >DEACTIVATE floor
//
//   static float InvertedAgentCount(int32 AgentCount, int32 AgentMax);
//     // Math.InverseLerp(0, AgentMax, AgentCount) clamped: return FMath::Clamp((float)AgentCount / (float)AgentMax, 0.f, 1.f);
//
//   static float VomitChancePerTick(int32 AgentCount, int32 AgentMax, float DeltaT);
//     // const float inv = InvertedAgentCount(AgentCount, AgentMax);
//     // const float occ = FMath::Lerp(2.0f, 5.0f, inv);   // VOMIT_OCCURRENCES_MIN..MAX
//     // return (occ / 300.0f) * DeltaT;
//
//   static float VomitDuration(int32 AgentCount, int32 AgentMax);
//     // return FMath::Max(3.0f, 10.0f * InvertedAgentCount(AgentCount, AgentMax));
//
//   static int32 VomitWaterDrain();      // return 70;   // WATER_DRAIN_FROM_VOMIT
//   static int32 VomitEnergyDrain();     // return 55;   // ENERGY_DRAIN_FROM_VOMIT
//   static int32 AgentReducePerVomit();  // return 60;   // ReduceAgent(FOOD_POISON, 60)
//
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Survival/EmergePoisoning.h"

BEGIN_DEFINE_SPEC(FEmergePoisoningSpec, "Emergence.Survival.Poisoning",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergePoisoningSpec)
void FEmergePoisoningSpec::Define()
{
	Describe("Toxicity", [this]()
	{
		It("decays 1/sec and forces a vomit strictly above threshold 70", [this]()
		{
			TestEqual(TEXT("decay dt=1 -> -1.0"), UEmergePoisoning::ToxicityDecayPerTick(1.0f), -1.0f);
			TestEqual(TEXT("decay dt=2 -> -2.0"), UEmergePoisoning::ToxicityDecayPerTick(2.0f), -2.0f);
			TestTrue(TEXT("71 vomits"), UEmergePoisoning::ShouldVomitFromToxicity(71.0f));
			TestFalse(TEXT("70 does not (strictly >)"), UEmergePoisoning::ShouldVomitFromToxicity(70.0f));
			TestFalse(TEXT("50 does not"), UEmergePoisoning::ShouldVomitFromToxicity(50.0f));
		});
	});

	Describe("Poisoning activation", [this]()
	{
		It("active at agent>=1; vomiting only above the 40 floor", [this]()
		{
			TestTrue(TEXT("1 agent active"), UEmergePoisoning::PoisoningActive(1));
			TestFalse(TEXT("0 agent inactive"), UEmergePoisoning::PoisoningActive(0));
			TestFalse(TEXT("40 no vomiting (floor)"), UEmergePoisoning::VomitingActive(40));
			TestTrue(TEXT("41 vomiting"), UEmergePoisoning::VomitingActive(41));
			TestFalse(TEXT("20 no vomiting"), UEmergePoisoning::VomitingActive(20));
		});
	});

	Describe("InvertedAgentCount", [this]()
	{
		It("agent/max clamped to [0,1] (FOOD_POISON max 800)", [this]()
		{
			TestEqual(TEXT("400/800 -> 0.5"), UEmergePoisoning::InvertedAgentCount(400, 800), 0.5f);
			TestEqual(TEXT("800/800 -> 1.0"), UEmergePoisoning::InvertedAgentCount(800, 800), 1.0f);
			TestEqual(TEXT("0/800 -> 0.0"), UEmergePoisoning::InvertedAgentCount(0, 800), 0.0f);
			TestEqual(TEXT("200/800 -> 0.25"), UEmergePoisoning::InvertedAgentCount(200, 800), 0.25f);
			TestEqual(TEXT("over max clamps to 1"), UEmergePoisoning::InvertedAgentCount(1000, 800), 1.0f);
		});
	});

	Describe("Vomit chance & duration", [this]()
	{
		It("occurrence Lerp(2,5) over agent load, /300 per sec; duration max(3, 10*inv)", [this]()
		{
			TestEqual(TEXT("empty load dt=1 -> 2/300"), UEmergePoisoning::VomitChancePerTick(0, 800, 1.0f), 2.0f / 300.0f);
			TestEqual(TEXT("full load dt=1 -> 5/300"), UEmergePoisoning::VomitChancePerTick(800, 800, 1.0f), 5.0f / 300.0f);
			TestEqual(TEXT("half load dt=1 -> 3.5/300"), UEmergePoisoning::VomitChancePerTick(400, 800, 1.0f), 3.5f / 300.0f);
			TestEqual(TEXT("full load dt=2 -> 2x"), UEmergePoisoning::VomitChancePerTick(800, 800, 2.0f), (5.0f / 300.0f) * 2.0f);
			TestEqual(TEXT("duration full -> 10"), UEmergePoisoning::VomitDuration(800, 800), 10.0f);
			TestEqual(TEXT("duration empty -> floor 3"), UEmergePoisoning::VomitDuration(0, 800), 3.0f);
			TestEqual(TEXT("duration quarter -> floor 3 (2.5<3)"), UEmergePoisoning::VomitDuration(200, 800), 3.0f);
			TestEqual(TEXT("duration half -> 5"), UEmergePoisoning::VomitDuration(400, 800), 5.0f);
		});
	});

	Describe("Vomit drains (exact DayZ constants)", [this]()
	{
		It("water 70, energy 55, agent reduce 60", [this]()
		{
			TestEqual(TEXT("water drain 70"), UEmergePoisoning::VomitWaterDrain(), 70);
			TestEqual(TEXT("energy drain 55"), UEmergePoisoning::VomitEnergyDrain(), 55);
			TestEqual(TEXT("agent reduce 60"), UEmergePoisoning::AgentReducePerVomit(), 60);
		});
	});
}
#endif
