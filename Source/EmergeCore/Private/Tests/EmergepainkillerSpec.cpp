// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: painkiller / morphine (survival medical).
// Meds dull pain for a duration; morphine fully suppresses even fracture pain (letting you move on a broken leg).
// Composes with EmergeFracture. TUNABLE design values, fixed deterministic model.
// CONTRACT — NEW class UEmergePainkiller : public UBlueprintFunctionLibrary
//   in Public/Survival/EmergePainkiller.h + Private/Survival/EmergePainkiller.cpp, static UFUNCTIONs.
//   DrugType int32: 0 None, 1 Painkiller, 2 Morphine.
//
//   static float PainReduction(int32 DrugType);   // switch: 1 -> 0.5f; 2 -> 1.0f; default -> 0.0f;
//   static float DurationSec(int32 DrugType);      // switch: 1 -> 300.0f; 2 -> 600.0f; default -> 0.0f;
//   static bool  SuppressesFracturePain(int32 DrugType);   // return DrugType == 2;  // morphine only
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
	Describe("PainReduction & Duration", [this]()
	{
		It("morphine stronger and longer than painkillers", [this]()
		{
			TestEqual(TEXT("none 0"), UEmergePainkiller::PainReduction(0), 0.0f);
			TestEqual(TEXT("painkiller 0.5"), UEmergePainkiller::PainReduction(1), 0.5f);
			TestEqual(TEXT("morphine 1.0"), UEmergePainkiller::PainReduction(2), 1.0f);
			TestEqual(TEXT("painkiller 300s"), UEmergePainkiller::DurationSec(1), 300.0f);
			TestEqual(TEXT("morphine 600s"), UEmergePainkiller::DurationSec(2), 600.0f);
			TestEqual(TEXT("none 0s"), UEmergePainkiller::DurationSec(0), 0.0f);
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
