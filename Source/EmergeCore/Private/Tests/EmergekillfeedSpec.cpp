// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: killfeed entry view-model (shell).
// SHELL stratum: decodes a kill event's flag bitmask into display hints (headshot, wallbang) and picks how long
// the killfeed line lingers. Pure bit/view logic; deterministic.
// CONTRACT — NEW class UEmergeKillfeed : public UBlueprintFunctionLibrary
//   in Public/Shell/EmergeKillfeed.h + Private/Shell/EmergeKillfeed.cpp, static UFUNCTIONs.
//   KillFlags bitmask: bit0 (1) headshot, bit1 (2) wallbang/penetration.
//
//   static bool  IsHeadshotKill(int32 KillFlags);   // return (KillFlags & 1) != 0;
//   static bool  IsWallbang(int32 KillFlags);        // return (KillFlags & 2) != 0;
//   static float EntryLifetimeSec(bool bImportant);   // return bImportant ? 8.0f : 5.0f;
//
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Shell/EmergeKillfeed.h"

BEGIN_DEFINE_SPEC(FEmergeKillfeedSpec, "Emergence.Shell.Killfeed",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeKillfeedSpec)
void FEmergeKillfeedSpec::Define()
{
	Describe("Flag decode & lifetime", [this]()
	{
		It("reads headshot/wallbang bits and entry lifetime", [this]()
		{
			TestTrue(TEXT("bit0 headshot"), UEmergeKillfeed::IsHeadshotKill(1));
			TestFalse(TEXT("no headshot"), UEmergeKillfeed::IsHeadshotKill(0));
			TestTrue(TEXT("headshot within combined flags"), UEmergeKillfeed::IsHeadshotKill(3));
			TestTrue(TEXT("bit1 wallbang"), UEmergeKillfeed::IsWallbang(2));
			TestFalse(TEXT("no wallbang"), UEmergeKillfeed::IsWallbang(1));
			TestTrue(TEXT("wallbang within combined flags"), UEmergeKillfeed::IsWallbang(3));
			TestEqual(TEXT("important lingers 8s"), UEmergeKillfeed::EntryLifetimeSec(true), 8.0f);
			TestEqual(TEXT("normal 5s"), UEmergeKillfeed::EntryLifetimeSec(false), 5.0f);
		});
	});
}
#endif
