// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: compass HUD view-model.
// SHELL stratum: a DayZ-style compass/heading readout. Normalizes an arbitrary yaw to [0,360) and maps it to an
// 8-point cardinal index (0=N,1=NE,2=E,3=SE,4=S,5=SW,6=W,7=NW) using the standard 45-degree sectors (a bearing
// rounds to the nearest sector, wrapping at 360). Universal navigation math; deterministic.
// CONTRACT — NEW class UEmergeCompassHud : public UBlueprintFunctionLibrary
//   in Public/Shell/EmergeCompassHud.h + Private/Shell/EmergeCompassHud.cpp, static UFUNCTIONs.
//
//   static float NormalizeHeading(float Degrees);
//     // wrap into [0,360): float d = FMath::Fmod(Degrees, 360.0f); if (d < 0.0f) d += 360.0f; return d;
//   static int32 Cardinal8(float Degrees);
//     // 8-point sector, nearest, wrapping: return ((int32)FMath::RoundToInt(NormalizeHeading(Degrees) / 45.0f)) % 8;
//
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Shell/EmergeCompassHud.h"

BEGIN_DEFINE_SPEC(FEmergeCompassHudSpec, "Emergence.Shell.CompassHud",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeCompassHudSpec)
void FEmergeCompassHudSpec::Define()
{
	Describe("NormalizeHeading", [this]()
	{
		It("wraps any yaw into [0,360)", [this]()
		{
			TestEqual(TEXT("0 -> 0"), UEmergeCompassHud::NormalizeHeading(0.f), 0.0f);
			TestEqual(TEXT("360 -> 0"), UEmergeCompassHud::NormalizeHeading(360.f), 0.0f);
			TestEqual(TEXT("450 -> 90"), UEmergeCompassHud::NormalizeHeading(450.f), 90.0f);
			TestEqual(TEXT("-90 -> 270"), UEmergeCompassHud::NormalizeHeading(-90.f), 270.0f);
			TestEqual(TEXT("270 stays"), UEmergeCompassHud::NormalizeHeading(270.f), 270.0f);
		});
	});

	Describe("Cardinal8", [this]()
	{
		It("maps a bearing to the nearest 8-point sector, wrapping at 360", [this]()
		{
			TestEqual(TEXT("0 N"), UEmergeCompassHud::Cardinal8(0.f), 0);
			TestEqual(TEXT("45 NE"), UEmergeCompassHud::Cardinal8(45.f), 1);
			TestEqual(TEXT("90 E"), UEmergeCompassHud::Cardinal8(90.f), 2);
			TestEqual(TEXT("135 SE"), UEmergeCompassHud::Cardinal8(135.f), 3);
			TestEqual(TEXT("180 S"), UEmergeCompassHud::Cardinal8(180.f), 4);
			TestEqual(TEXT("225 SW"), UEmergeCompassHud::Cardinal8(225.f), 5);
			TestEqual(TEXT("270 W"), UEmergeCompassHud::Cardinal8(270.f), 6);
			TestEqual(TEXT("315 NW"), UEmergeCompassHud::Cardinal8(315.f), 7);
			TestEqual(TEXT("359 wraps to N"), UEmergeCompassHud::Cardinal8(359.f), 0);
			TestEqual(TEXT("23 rounds to NE"), UEmergeCompassHud::Cardinal8(23.f), 1);
			TestEqual(TEXT("22 rounds to N"), UEmergeCompassHud::Cardinal8(22.f), 0);
		});
	});
}
#endif
