// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: vital HUD view-model (DayZ-style health panel).
// THE SHELL PIVOT: the deep survival sim is invisible without a HUD. This is the pure view-model logic a
// DayZ-style health/status panel needs - map raw vital values to bar fills, status levels, color tiers, and
// badge visibility - with NO UMG/art, fully unit-testable. Numbers exact from decompiled DayZ playerconstants.c
// (SL_* thresholds) + estatlevels.c ordinal ordering (GREAT 0, HIGH 1, MEDIUM 2, LOW 3, CRITICAL 4).
// CONTRACT — NEW class UEmergeVitalHud : public UBlueprintFunctionLibrary
//   in Public/Shell/EmergeVitalHud.h + Private/Shell/EmergeVitalHud.cpp, static UFUNCTIONs.
//   Stat level is the EStatLevels ordinal (0 GREAT best .. 4 CRITICAL worst).
//
//   static float BarFill01(float Value, float Max);
//     // return FMath::Clamp(Value / Max, 0.0f, 1.0f);
//
//   // GetStatLevel: value <= critical -> 4; <= low -> 3; <= normal -> 2; <= high -> 1; else 0.
//   static int32 BloodStatLevel(float Blood);    // thresholds 3000/3500/4000/4500  (SL_BLOOD_*)
//   static int32 HealthStatLevel(float Health);  // thresholds 15/30/50/80          (SL_HEALTH_*)
//   static int32 EnergyStatLevel(float Energy);  // thresholds 0/300/800/3500       (SL_ENERGY_*)
//   static int32 WaterStatLevel(float Water);    // thresholds 0/300/800/3500       (SL_WATER_*)
//
//   static int32 DisplayColorTier(int32 StatLevel);
//     // HUD color band: CRITICAL(4)->3 danger, LOW(3)->2 warning, MEDIUM(2)->1 caution, HIGH(1)/GREAT(0)->0 ok.
//     // if (StatLevel >= 4) return 3; if (StatLevel == 3) return 2; if (StatLevel == 2) return 1; return 0;
//
//   static bool ShowLowVitalBadge(int32 StatLevel);
//     // A warning badge appears once a vital is LOW or worse. return StatLevel >= 3;
//
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Shell/EmergeVitalHud.h"

BEGIN_DEFINE_SPEC(FEmergeVitalHudSpec, "Emergence.Shell.VitalHud",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeVitalHudSpec)
void FEmergeVitalHudSpec::Define()
{
	Describe("BarFill01", [this]()
	{
		It("value/max clamped to [0,1]", [this]()
		{
			TestEqual(TEXT("half"), UEmergeVitalHud::BarFill01(2500.f, 5000.f), 0.5f);
			TestEqual(TEXT("full"), UEmergeVitalHud::BarFill01(5000.f, 5000.f), 1.0f);
			TestEqual(TEXT("empty"), UEmergeVitalHud::BarFill01(0.f, 5000.f), 0.0f);
			TestEqual(TEXT("over max clamps"), UEmergeVitalHud::BarFill01(6000.f, 5000.f), 1.0f);
		});
	});

	Describe("Per-vital stat levels (exact DayZ SL_* thresholds)", [this]()
	{
		It("blood 3000/3500/4000/4500", [this]()
		{
			TestEqual(TEXT("2900 critical"), UEmergeVitalHud::BloodStatLevel(2900.f), 4);
			TestEqual(TEXT("3000 critical (boundary)"), UEmergeVitalHud::BloodStatLevel(3000.f), 4);
			TestEqual(TEXT("3500 low"), UEmergeVitalHud::BloodStatLevel(3500.f), 3);
			TestEqual(TEXT("4000 medium"), UEmergeVitalHud::BloodStatLevel(4000.f), 2);
			TestEqual(TEXT("4500 high"), UEmergeVitalHud::BloodStatLevel(4500.f), 1);
			TestEqual(TEXT("5000 great"), UEmergeVitalHud::BloodStatLevel(5000.f), 0);
		});
		It("health 15/30/50/80", [this]()
		{
			TestEqual(TEXT("15 critical"), UEmergeVitalHud::HealthStatLevel(15.f), 4);
			TestEqual(TEXT("30 low"), UEmergeVitalHud::HealthStatLevel(30.f), 3);
			TestEqual(TEXT("50 medium"), UEmergeVitalHud::HealthStatLevel(50.f), 2);
			TestEqual(TEXT("80 high"), UEmergeVitalHud::HealthStatLevel(80.f), 1);
			TestEqual(TEXT("100 great"), UEmergeVitalHud::HealthStatLevel(100.f), 0);
		});
		It("energy & water 0/300/800/3500", [this]()
		{
			TestEqual(TEXT("energy 0 critical"), UEmergeVitalHud::EnergyStatLevel(0.f), 4);
			TestEqual(TEXT("energy 300 low"), UEmergeVitalHud::EnergyStatLevel(300.f), 3);
			TestEqual(TEXT("energy 800 medium"), UEmergeVitalHud::EnergyStatLevel(800.f), 2);
			TestEqual(TEXT("energy 3500 high"), UEmergeVitalHud::EnergyStatLevel(3500.f), 1);
			TestEqual(TEXT("energy 5000 great"), UEmergeVitalHud::EnergyStatLevel(5000.f), 0);
			TestEqual(TEXT("water 300 low"), UEmergeVitalHud::WaterStatLevel(300.f), 3);
			TestEqual(TEXT("water 5000 great"), UEmergeVitalHud::WaterStatLevel(5000.f), 0);
		});
	});

	Describe("DisplayColorTier & badge", [this]()
	{
		It("maps stat level to a HUD color band and low-vital badge", [this]()
		{
			TestEqual(TEXT("critical -> danger 3"), UEmergeVitalHud::DisplayColorTier(4), 3);
			TestEqual(TEXT("low -> warning 2"), UEmergeVitalHud::DisplayColorTier(3), 2);
			TestEqual(TEXT("medium -> caution 1"), UEmergeVitalHud::DisplayColorTier(2), 1);
			TestEqual(TEXT("high -> ok 0"), UEmergeVitalHud::DisplayColorTier(1), 0);
			TestEqual(TEXT("great -> ok 0"), UEmergeVitalHud::DisplayColorTier(0), 0);
			TestTrue(TEXT("critical shows badge"), UEmergeVitalHud::ShowLowVitalBadge(4));
			TestTrue(TEXT("low shows badge"), UEmergeVitalHud::ShowLowVitalBadge(3));
			TestFalse(TEXT("medium no badge"), UEmergeVitalHud::ShowLowVitalBadge(2));
			TestFalse(TEXT("great no badge"), UEmergeVitalHud::ShowLowVitalBadge(0));
		});
	});
}
#endif
