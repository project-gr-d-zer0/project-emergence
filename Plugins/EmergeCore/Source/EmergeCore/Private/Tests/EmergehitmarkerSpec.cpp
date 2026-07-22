// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: hitmarker view-model (shell/combat HUD).
// SHELL stratum: the hit-feedback marker. Given the outcome of a shot it picks the marker state (none/hit/armor/
// headshot/kill, by priority) and a scale so kills and headshots pop. Pure view logic, no UMG; deterministic.
// CONTRACT — NEW class UEmergeHitmarker : public UBlueprintFunctionLibrary
//   in Public/Shell/EmergeHitmarker.h + Private/Shell/EmergeHitmarker.cpp, static UFUNCTIONs.
//   State int32: 0 None, 1 Hit, 2 Armor, 3 Headshot, 4 Kill.
//
//   static int32 MarkerState(bool bHit, bool bArmor, bool bHeadshot, bool bKill);
//     // if (!bHit) return 0;  if (bKill) return 4;  if (bHeadshot) return 3;  if (bArmor) return 2;  return 1;
//   static float MarkerScale(int32 State);
//     // return State == 4 ? 1.5f : (State == 3 ? 1.25f : 1.0f);
//
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Shell/EmergeHitmarker.h"

BEGIN_DEFINE_SPEC(FEmergeHitmarkerSpec, "Emergence.Shell.Hitmarker",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeHitmarkerSpec)
void FEmergeHitmarkerSpec::Define()
{
	Describe("MarkerState", [this]()
	{
		It("selects the marker by outcome priority (kill > headshot > armor > hit)", [this]()
		{
			TestEqual(TEXT("miss -> none"), UEmergeHitmarker::MarkerState(false, false, false, false), 0);
			TestEqual(TEXT("plain hit"), UEmergeHitmarker::MarkerState(true, false, false, false), 1);
			TestEqual(TEXT("armor absorb"), UEmergeHitmarker::MarkerState(true, true, false, false), 2);
			TestEqual(TEXT("headshot"), UEmergeHitmarker::MarkerState(true, false, true, false), 3);
			TestEqual(TEXT("kill outranks all"), UEmergeHitmarker::MarkerState(true, true, true, true), 4);
		});
	});

	Describe("MarkerScale", [this]()
	{
		It("kills and headshots pop larger", [this]()
		{
			TestEqual(TEXT("kill 1.5"), UEmergeHitmarker::MarkerScale(4), 1.5f);
			TestEqual(TEXT("headshot 1.25"), UEmergeHitmarker::MarkerScale(3), 1.25f);
			TestEqual(TEXT("hit 1.0"), UEmergeHitmarker::MarkerScale(1), 1.0f);
			TestEqual(TEXT("none 1.0"), UEmergeHitmarker::MarkerScale(0), 1.0f);
		});
	});
}
#endif
