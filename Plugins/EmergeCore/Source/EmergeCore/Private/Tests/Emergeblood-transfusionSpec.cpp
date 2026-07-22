// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: blood-type transfusion (DayZ-style).
// Transfusing the wrong blood type triggers a hemolytic reaction. ABO/Rh compatibility is standard medical logic
// (O- universal donor, AB+ universal recipient, Rh- can't take Rh+); the reaction thresholds and drain are exact
// from decompiled DayZ playerconstants.c: incompatible blood > 75 mL -> shock, > 175 mL -> hemolytic reaction,
// HEMOLYTIC_BLOOD_DRAIN_PER_SEC 7. Composes with EmergeBloodRegen / EmergeShock.
// CONTRACT — NEW class UEmergeBloodTransfusion : public UBlueprintFunctionLibrary
//   in Public/Survival/EmergeBloodTransfusion.h + Private/Survival/EmergeBloodTransfusion.cpp, static UFUNCTIONs.
//   ABO int32 as an antigen bitmask: 0 O (none), 1 A, 2 B, 3 AB (A|B).
//
//   static bool IsCompatible(int32 DonorAbo, bool bDonorRhPos, int32 RecipAbo, bool bRecipRhPos);
//     // ABO: donor antigens must be a subset of recipient antigens -> (DonorAbo & RecipAbo) == DonorAbo
//     // Rh: an Rh- recipient cannot receive Rh+ blood -> (bRecipRhPos || !bDonorRhPos)
//     // return ((DonorAbo & RecipAbo) == DonorAbo) && (bRecipRhPos || !bDonorRhPos);
//   static int32 TransfusionReaction(bool bCompatible, float BloodAmount);
//     // if (bCompatible) return 0;                       // 0 none, 1 shock, 2 hemolytic reaction
//     // if (BloodAmount > 175.0f) return 2;              // HEMOLYTIC_REACTION_THRESHOLD
//     // if (BloodAmount > 75.0f)  return 1;              // HEMOLYTIC_RISK_SHOCK_THRESHOLD
//     // return 0;
//   static float HemolyticDrainPerTick(float DeltaT);   // return 7.0f * DeltaT;  // HEMOLYTIC_BLOOD_DRAIN_PER_SEC
//
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Survival/EmergeBloodTransfusion.h"

BEGIN_DEFINE_SPEC(FEmergeBloodTransfusionSpec, "Emergence.Survival.BloodTransfusion",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeBloodTransfusionSpec)
void FEmergeBloodTransfusionSpec::Define()
{
	Describe("IsCompatible", [this]()
	{
		It("ABO subset + Rh rules", [this]()
		{
			TestTrue(TEXT("O- universal donor -> AB+"), UEmergeBloodTransfusion::IsCompatible(0, false, 3, true));
			TestTrue(TEXT("O- -> O-"), UEmergeBloodTransfusion::IsCompatible(0, false, 0, false));
			TestTrue(TEXT("A+ -> AB+"), UEmergeBloodTransfusion::IsCompatible(1, true, 3, true));
			TestFalse(TEXT("A+ -> O- (wrong ABO)"), UEmergeBloodTransfusion::IsCompatible(1, true, 0, false));
			TestFalse(TEXT("O+ -> O- (Rh mismatch)"), UEmergeBloodTransfusion::IsCompatible(0, true, 0, false));
			TestFalse(TEXT("A -> B"), UEmergeBloodTransfusion::IsCompatible(1, true, 2, true));
			TestTrue(TEXT("B+ -> B+"), UEmergeBloodTransfusion::IsCompatible(2, true, 2, true));
		});
	});

	Describe("TransfusionReaction & drain", [this]()
	{
		It("incompatible blood over thresholds -> shock/hemolytic", [this]()
		{
			TestEqual(TEXT("compatible -> none"), UEmergeBloodTransfusion::TransfusionReaction(true, 500.f), 0);
			TestEqual(TEXT("incompatible 50mL -> tolerated"), UEmergeBloodTransfusion::TransfusionReaction(false, 50.f), 0);
			TestEqual(TEXT("incompatible 75mL -> tolerated (not >75)"), UEmergeBloodTransfusion::TransfusionReaction(false, 75.f), 0);
			TestEqual(TEXT("incompatible 100mL -> shock"), UEmergeBloodTransfusion::TransfusionReaction(false, 100.f), 1);
			TestEqual(TEXT("incompatible 175mL -> shock (not >175)"), UEmergeBloodTransfusion::TransfusionReaction(false, 175.f), 1);
			TestEqual(TEXT("incompatible 200mL -> hemolytic"), UEmergeBloodTransfusion::TransfusionReaction(false, 200.f), 2);
			TestEqual(TEXT("drain 7/s"), UEmergeBloodTransfusion::HemolyticDrainPerTick(1.0f), 7.0f);
			TestEqual(TEXT("drain dt=2 -> 14"), UEmergeBloodTransfusion::HemolyticDrainPerTick(2.0f), 14.0f);
		});
	});
}
#endif
