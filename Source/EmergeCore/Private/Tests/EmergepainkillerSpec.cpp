// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: painkiller / morphine (survival medical). [CORPUS-FIX]
// SOURCE-CORRECTED to DayZ: painkillersmdfr.c LIFETIME=240, OnActivate SetShockMultiplier(0.5); morphinemdfr.c
// LIFETIME=60, SetShockMultiplier(0.1). Prior values (300/600, morphine longer, morphine reduction 1.0) were
// invented + inverted. Morphine is the SHORTER, STRONGER one; it also suppresses fracture pain.
// CONTRACT — NEW class UEmergePainkiller : public UBlueprintFunctionLibrary
//   in Public/Survival/EmergePainkiller.h + Private/Survival/EmergePainkiller.cpp, static UFUNCTIONs.
//   DrugType int32: 0 None, 1 Painkiller, 2 Morphine.
//
//   static float PainReduction(int32 DrugType);   // 1 -> 0.5f (shock mult 0.5); 2 -> 0.9f (shock mult 0.1); default 0.0f
//   static float DurationSec(int32 DrugType);      // 1 -> 240.0f; 2 -> 60.0f; default 0.0f   (DayZ LIFETIME)
//   static bool  SuppressesFracturePain(int32 DrugType);   // return DrugType == 2;  // morphine
//
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Survival/EmergePainkiller.h"

BEGIN_DEFINE_SPEC(FEmergePainkillerSpec, "Emergence.Survival.Painkiller",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergePainkillerSpec)
void FEmergePainkillerSpec::Define()
{
	Describe("PainReduction & Duration (DayZ source values)", [this]()
	{
		It("morphine stronger AND shorter than painkillers", [this]()
		{
			TestEqual(TEXT("none 0"), UEmergePainkiller::PainReduction(0), 0.0f);
			TestEqual(TEXT("painkiller 0.5 (shock mult 0.5)"), UEmergePainkiller::PainReduction(1), 0.5f);
			TestEqual(TEXT("morphine 0.9 (shock mult 0.1)"), UEmergePainkiller::PainReduction(2), 0.9f);
			TestEqual(TEXT("painkiller LIFETIME 240s"), UEmergePainkiller::DurationSec(1), 240.0f);
			TestEqual(TEXT("morphine LIFETIME 60s"), UEmergePainkiller::DurationSec(2), 60.0f);
			TestEqual(TEXT("none 0s"), UEmergePainkiller::DurationSec(0), 0.0f);
			TestTrue(TEXT("morphine is the shorter one"), UEmergePainkiller::DurationSec(2) < UEmergePainkiller::DurationSec(1));
		});
	});

	Describe("SuppressesFracturePain", [this]()
	{
		It("only morphine masks fracture pain", [this]()
		{
			TestTrue(TEXT("morphine suppresses"), UEmergePainkiller::SuppressesFracturePain(2));
			TestFalse(TEXT("painkiller does not"), UEmergePainkiller::SuppressesFracturePain(1));
			TestFalse(TEXT("none does not"), UEmergePainkiller::SuppressesFracturePain(0));
		});
	});
}
#endif
