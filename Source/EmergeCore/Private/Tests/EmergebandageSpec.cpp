// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: bandaging (DayZ-style) — the counterpart that
// CLOSES the wound loop opened by EmergeBleeding. Applying a bandage removes the single most-significant bleeding
// source, takes a fixed time, and a dirty dressing carries a wound-infection chance (feeding EmergeDisease's
// agent system); each application consumes one unit. Numbers exact from decompiled DayZ actionbandagebase.c
// (RemoveMostSignificantBleedingSource = 1 per apply; AddQuantity(-1)), actionconstants.c (UATimeSpent.BANDAGE 4s),
// bandagedressing.c GetInfectionChance (clean 0.0, dirty 0.05).
// CONTRACT — NEW class UEmergeBandage : public UBlueprintFunctionLibrary
//   in Public/Survival/EmergeBandage.h + Private/Survival/EmergeBandage.cpp, static UFUNCTIONs.
//
//   static int32 SourcesClosedPerApplication();     // return 1;   // removes the most significant source only
//   static int32 RemainingSources(int32 SourcesBefore);  // return FMath::Max(0, SourcesBefore - 1);
//   static bool  StopsAllBleeding(int32 SourcesBefore);  // return SourcesBefore <= 1;
//   static float BandageTimeSec();                   // return 4.0f;   // UATimeSpent.BANDAGE
//   static float DressingInfectionChance(bool bClean); // return bClean ? 0.0f : 0.05f;
//   static int32 QuantityConsumed();                 // return 1;   // AddQuantity(-1)
//
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Survival/EmergeBandage.h"

BEGIN_DEFINE_SPEC(FEmergeBandageSpec, "Emergence.Survival.Bandage",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeBandageSpec)
void FEmergeBandageSpec::Define()
{
	Describe("Closing bleeding sources", [this]()
	{
		It("removes exactly one (the most significant) source per application", [this]()
		{
			TestEqual(TEXT("closes 1 per apply"), UEmergeBandage::SourcesClosedPerApplication(), 1);
			TestEqual(TEXT("3 sources -> 2 remain"), UEmergeBandage::RemainingSources(3), 2);
			TestEqual(TEXT("1 source -> 0 remain"), UEmergeBandage::RemainingSources(1), 0);
			TestEqual(TEXT("0 sources -> 0 (no underflow)"), UEmergeBandage::RemainingSources(0), 0);
		});
		It("stops all bleeding only when a single source remained", [this]()
		{
			TestTrue(TEXT("1 source fully stops"), UEmergeBandage::StopsAllBleeding(1));
			TestTrue(TEXT("0 sources trivially stopped"), UEmergeBandage::StopsAllBleeding(0));
			TestFalse(TEXT("2 sources still bleeding"), UEmergeBandage::StopsAllBleeding(2));
		});
	});

	Describe("Timing, infection & consumption", [this]()
	{
		It("fixed bandage time, clean vs dirty infection chance, consumes one unit", [this]()
		{
			TestEqual(TEXT("bandage takes 4s"), UEmergeBandage::BandageTimeSec(), 4.0f);
			TestEqual(TEXT("clean dressing 0 infection"), UEmergeBandage::DressingInfectionChance(true), 0.0f);
			TestEqual(TEXT("dirty dressing 0.05 infection"), UEmergeBandage::DressingInfectionChance(false), 0.05f);
			TestEqual(TEXT("consumes 1 unit"), UEmergeBandage::QuantityConsumed(), 1);
		});
	});
}
#endif
