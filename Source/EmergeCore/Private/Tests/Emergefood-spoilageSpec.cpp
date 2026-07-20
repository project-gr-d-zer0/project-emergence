// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: food spoilage (Project Zomboid).
// Food ages in days toward Stale (OffAge) then Rotten (OffAgeMax). Cold storage slows aging — but only
// while the appliance has power, which ties this directly to the utility timeline (EmergeUtilityGrid):
// once electricity cuts, a fridge ages food at the room rate. Numbers are exact from decompiled PZ B41
// Food.java: frozen = 0.02x, powered fridge = 0.2x (default FridgeFactor), FoodRotSpeed preset multiplier
// {1:1.7, 2:1.4, 3:1.0, 4:0.7, 5:0.4}. Age accrues as ElapsedHours * storageRate * rotSpeed / 24.
// CONTRACT — NEW class UEmergeFoodSpoilage : public UBlueprintFunctionLibrary
//   in Public/Survival/EmergeFoodSpoilage.h + Private/Survival/EmergeFoodSpoilage.cpp, static UFUNCTIONs:
//
//   static int32 FreshnessState(float Age, float OffAge, float OffAgeMax);
//     // 0 = Fresh, 1 = Stale, 2 = Rotten.
//     // if (Age >= OffAgeMax) return 2; if (Age < OffAge) return 0; return 1;
//
//   static float RotSpeedMult(int32 FoodRotSpeedPreset);
//     // PZ FoodRotSpeed sandbox multiplier. return 1.7f/1.4f/0.7f/0.4f for presets 1/2/4/5; else 1.0f.
//
//   static float StorageRateMult(int32 StorageKind, bool bPowered);
//     // StorageKind: 0 = Room, 1 = Fridge, 2 = Freezer. Cold storage only helps while powered.
//     // Room -> 1.0f. Fridge -> bPowered ? 0.2f : 1.0f. Freezer -> bPowered ? 0.02f : 1.0f.
//     // Any other StorageKind -> 1.0f.
//
//   static float AgeDeltaDays(float ElapsedHours, int32 StorageKind, bool bPowered, int32 FoodRotSpeedPreset);
//     // return ElapsedHours * StorageRateMult(StorageKind, bPowered)
//     //      * RotSpeedMult(FoodRotSpeedPreset) / 24.0f;
//
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Survival/EmergeFoodSpoilage.h"

BEGIN_DEFINE_SPEC(FEmergeFoodSpoilageSpec, "Emergence.Survival.FoodSpoilage",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeFoodSpoilageSpec)
void FEmergeFoodSpoilageSpec::Define()
{
	Describe("FreshnessState", [this]()
	{
		It("fresh below OffAge, stale in band, rotten at/above OffAgeMax", [this]()
		{
			TestEqual(TEXT("young -> fresh"), UEmergeFoodSpoilage::FreshnessState(0.5f, 1.f, 3.f), 0);
			TestEqual(TEXT("Age==OffAge -> stale"), UEmergeFoodSpoilage::FreshnessState(1.f, 1.f, 3.f), 1);
			TestEqual(TEXT("mid band -> stale"), UEmergeFoodSpoilage::FreshnessState(2.9f, 1.f, 3.f), 1);
			TestEqual(TEXT("Age==OffAgeMax -> rotten"), UEmergeFoodSpoilage::FreshnessState(3.f, 1.f, 3.f), 2);
			TestEqual(TEXT("old -> rotten"), UEmergeFoodSpoilage::FreshnessState(5.f, 1.f, 3.f), 2);
		});
	});

	Describe("RotSpeedMult", [this]()
	{
		It("matches PZ FoodRotSpeed presets", [this]()
		{
			TestEqual(TEXT("p1"), UEmergeFoodSpoilage::RotSpeedMult(1), 1.7f);
			TestEqual(TEXT("p2"), UEmergeFoodSpoilage::RotSpeedMult(2), 1.4f);
			TestEqual(TEXT("p3 default"), UEmergeFoodSpoilage::RotSpeedMult(3), 1.0f);
			TestEqual(TEXT("p4"), UEmergeFoodSpoilage::RotSpeedMult(4), 0.7f);
			TestEqual(TEXT("p5"), UEmergeFoodSpoilage::RotSpeedMult(5), 0.4f);
			TestEqual(TEXT("unknown -> 1"), UEmergeFoodSpoilage::RotSpeedMult(0), 1.0f);
		});
	});

	Describe("StorageRateMult", [this]()
	{
		It("cold storage only helps while powered", [this]()
		{
			TestEqual(TEXT("room"), UEmergeFoodSpoilage::StorageRateMult(0, true), 1.0f);
			TestEqual(TEXT("fridge powered"), UEmergeFoodSpoilage::StorageRateMult(1, true), 0.2f);
			TestEqual(TEXT("fridge no power -> room"), UEmergeFoodSpoilage::StorageRateMult(1, false), 1.0f);
			TestEqual(TEXT("freezer powered"), UEmergeFoodSpoilage::StorageRateMult(2, true), 0.02f);
			TestEqual(TEXT("freezer no power -> room"), UEmergeFoodSpoilage::StorageRateMult(2, false), 1.0f);
			TestEqual(TEXT("unknown kind -> room"), UEmergeFoodSpoilage::StorageRateMult(9, true), 1.0f);
		});
	});

	Describe("AgeDeltaDays", [this]()
	{
		It("composes hours, storage and rot-speed; power loss ruins the fridge", [this]()
		{
			TestEqual(TEXT("room 24h default -> 1 day"), UEmergeFoodSpoilage::AgeDeltaDays(24.f, 0, true, 3), 1.0f);
			TestEqual(TEXT("fridge powered 24h -> 0.2"), UEmergeFoodSpoilage::AgeDeltaDays(24.f, 1, true, 3), 0.2f);
			TestEqual(TEXT("freezer powered 24h -> 0.02"), UEmergeFoodSpoilage::AgeDeltaDays(24.f, 2, true, 3), 0.02f);
			TestEqual(TEXT("fridge UNPOWERED 24h -> 1 day"), UEmergeFoodSpoilage::AgeDeltaDays(24.f, 1, false, 3), 1.0f);
			TestEqual(TEXT("room 24h fast-rot p1 -> 1.7"), UEmergeFoodSpoilage::AgeDeltaDays(24.f, 0, true, 1), 1.7f);
		});
	});
}
#endif
