// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: utility timeline (Project Zomboid
// water/power shutoff). Municipal water and electricity run for a fixed number of in-game days, then
// cut — the signature PZ progression that forces the survival economy (fridges warm, taps run dry).
// Numbers are lifted from decompiled PZ B41 SandboxOptions: power is ON while NightsSurvived <
// ElecShutModifier; the preset->day-range tables (randomWaterShut / randomElectricityShut) below are
// exact. Water can cut from day 0; electricity always lasts at least 14 days (a 14-day floor on presets 2-6).
// CONTRACT — NEW class UEmergeUtilityGrid : public UBlueprintFunctionLibrary
//   in Public/Survival/EmergeUtilityGrid.h + Private/Survival/EmergeUtilityGrid.cpp, static UFUNCTIONs:
//
//   static bool IsUtilityOn(int32 ShutoffDay, int32 NightsSurvived);
//     // A negative ShutoffDay means the utility never shuts off (infinite).
//     // return ShutoffDay < 0 || NightsSurvived < ShutoffDay;
//
//   static FIntPoint ShutoffDayRange(int32 UtilityKind, int32 Preset);
//     // UtilityKind: 0 = Water, 1 = Electricity. Returns (Min,Max) inclusive day range for the preset.
//     // Preset outside 2..8 returns (-1,-1) (never shuts off). Tables (exact, from PZ B41):
//     //   Electricity floor is 14; Water floor is 0. Presets 7 and 8 are identical for both.
//     //   Preset: 2      3      4       5       6        7         8
//     //   Water : 0-30   0-60   0-180   0-360   0-1800   60-180    180-360
//     //   Elec  : 14-30  14-60  14-180  14-360  14-1800  60-180    180-360
//     // Implement as: Max by preset = {2:30,3:60,4:180,5:360,6:1800,7:180,8:360}; else -1.
//     //   Min = (Preset==7)?60 : (Preset==8)?180 : (UtilityKind==1 ? 14 : 0), but only when Max>=0.
//     // Return FIntPoint(Min, Max); FIntPoint(-1,-1) when Max<0.
//
//   static int32 ResolveShutoffDay(int32 UtilityKind, int32 Preset, float Roll01);
//     // Deterministically pick a shutoff day inside the preset range. Roll01 is clamped to [0,1].
//     // const FIntPoint R = ShutoffDayRange(UtilityKind, Preset);
//     // if (R.X < 0) return -1;
//     // return R.X + FMath::RoundToInt(FMath::Clamp(Roll01, 0.f, 1.f) * float(R.Y - R.X));
//
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Survival/EmergeUtilityGrid.h"

BEGIN_DEFINE_SPEC(FEmergeUtilityGridSpec, "Emergence.Survival.UtilityGrid",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeUtilityGridSpec)
void FEmergeUtilityGridSpec::Define()
{
	Describe("IsUtilityOn", [this]()
	{
		It("negative shutoff never cuts; otherwise on strictly before the shutoff day", [this]()
		{
			TestTrue(TEXT("-1 shutoff, day 0 -> on"), UEmergeUtilityGrid::IsUtilityOn(-1, 0));
			TestTrue(TEXT("-1 shutoff, day 9999 -> on"), UEmergeUtilityGrid::IsUtilityOn(-1, 9999));
			TestTrue(TEXT("shutoff 14, day 13 -> on"), UEmergeUtilityGrid::IsUtilityOn(14, 13));
			TestFalse(TEXT("shutoff 14, day 14 -> off"), UEmergeUtilityGrid::IsUtilityOn(14, 14));
			TestFalse(TEXT("shutoff 14, day 20 -> off"), UEmergeUtilityGrid::IsUtilityOn(14, 20));
			TestFalse(TEXT("shutoff 0, day 0 -> instant off"), UEmergeUtilityGrid::IsUtilityOn(0, 0));
		});
	});

	Describe("ShutoffDayRange (Water = 0, Electricity = 1)", [this]()
	{
		It("water table matches PZ exactly", [this]()
		{
			TestEqual(TEXT("W p2"), UEmergeUtilityGrid::ShutoffDayRange(0, 2), FIntPoint(0, 30));
			TestEqual(TEXT("W p3"), UEmergeUtilityGrid::ShutoffDayRange(0, 3), FIntPoint(0, 60));
			TestEqual(TEXT("W p4"), UEmergeUtilityGrid::ShutoffDayRange(0, 4), FIntPoint(0, 180));
			TestEqual(TEXT("W p5"), UEmergeUtilityGrid::ShutoffDayRange(0, 5), FIntPoint(0, 360));
			TestEqual(TEXT("W p6"), UEmergeUtilityGrid::ShutoffDayRange(0, 6), FIntPoint(0, 1800));
			TestEqual(TEXT("W p7"), UEmergeUtilityGrid::ShutoffDayRange(0, 7), FIntPoint(60, 180));
			TestEqual(TEXT("W p8"), UEmergeUtilityGrid::ShutoffDayRange(0, 8), FIntPoint(180, 360));
		});
		It("electricity table matches PZ exactly (14-day floor on 2-6)", [this]()
		{
			TestEqual(TEXT("E p2"), UEmergeUtilityGrid::ShutoffDayRange(1, 2), FIntPoint(14, 30));
			TestEqual(TEXT("E p3"), UEmergeUtilityGrid::ShutoffDayRange(1, 3), FIntPoint(14, 60));
			TestEqual(TEXT("E p4"), UEmergeUtilityGrid::ShutoffDayRange(1, 4), FIntPoint(14, 180));
			TestEqual(TEXT("E p5"), UEmergeUtilityGrid::ShutoffDayRange(1, 5), FIntPoint(14, 360));
			TestEqual(TEXT("E p6"), UEmergeUtilityGrid::ShutoffDayRange(1, 6), FIntPoint(14, 1800));
			TestEqual(TEXT("E p7"), UEmergeUtilityGrid::ShutoffDayRange(1, 7), FIntPoint(60, 180));
			TestEqual(TEXT("E p8"), UEmergeUtilityGrid::ShutoffDayRange(1, 8), FIntPoint(180, 360));
		});
		It("out-of-range presets never shut off", [this]()
		{
			TestEqual(TEXT("W p1"), UEmergeUtilityGrid::ShutoffDayRange(0, 1), FIntPoint(-1, -1));
			TestEqual(TEXT("W p9"), UEmergeUtilityGrid::ShutoffDayRange(0, 9), FIntPoint(-1, -1));
			TestEqual(TEXT("E p0"), UEmergeUtilityGrid::ShutoffDayRange(1, 0), FIntPoint(-1, -1));
		});
	});

	Describe("ResolveShutoffDay", [this]()
	{
		It("interpolates deterministically across the range, clamps the roll", [this]()
		{
			TestEqual(TEXT("E p2 roll 0 -> 14"), UEmergeUtilityGrid::ResolveShutoffDay(1, 2, 0.f), 14);
			TestEqual(TEXT("E p2 roll 1 -> 30"), UEmergeUtilityGrid::ResolveShutoffDay(1, 2, 1.f), 30);
			TestEqual(TEXT("E p2 roll .5 -> 22"), UEmergeUtilityGrid::ResolveShutoffDay(1, 2, 0.5f), 22);
			TestEqual(TEXT("W p3 roll .5 -> 30"), UEmergeUtilityGrid::ResolveShutoffDay(0, 3, 0.5f), 30);
			TestEqual(TEXT("W p3 roll 1 -> 60"), UEmergeUtilityGrid::ResolveShutoffDay(0, 3, 1.f), 60);
			TestEqual(TEXT("roll above 1 clamps"), UEmergeUtilityGrid::ResolveShutoffDay(1, 2, 2.f), 30);
			TestEqual(TEXT("never-preset -> -1"), UEmergeUtilityGrid::ResolveShutoffDay(0, 1, 0.5f), -1);
		});
	});
}
#endif
