// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: safe-zone loot securing (the "extraction").
// Canonical design: die OUTSIDE a safe zone -> lose everything; die INSIDE a safe zone -> keep all gear.
// CONTRACT — NEW class UEmergeSafeZone : public UBlueprintFunctionLibrary
//   in Public/World/EmergeSafeZone.h + Private/World/EmergeSafeZone.cpp, static UFUNCTIONs:
//   static bool GearKeptOnDeath(bool bDiedInSafeZone);   // return bDiedInSafeZone
//   static int32 ValueAtRisk(int32 CarriedValue, bool bInSafeZone);
//     // loot value you would lose if you died right now: 0 inside a safe zone, else FMath::Max(0, CarriedValue)
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "World/EmergeSafeZone.h"

BEGIN_DEFINE_SPEC(FEmergeSafeZoneSpec, "Emergence.World.SafeZone",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeSafeZoneSpec)
void FEmergeSafeZoneSpec::Define()
{
	Describe("GearKeptOnDeath (kept only when death is inside a safe zone)", [this]()
	{
		It("matches the rule at both cases", [this]()
		{
			TestTrue(TEXT("die INSIDE safe zone -> keep gear"),  UEmergeSafeZone::GearKeptOnDeath(true));
			TestFalse(TEXT("die OUTSIDE -> lose everything"),    UEmergeSafeZone::GearKeptOnDeath(false));
		});
	});

	Describe("ValueAtRisk (0 inside a safe zone, full carried value outside)", [this]()
	{
		It("matches exact golden values and boundaries", [this]()
		{
			TestEqual(TEXT("inside safe zone -> 0 at risk"),        UEmergeSafeZone::ValueAtRisk(100, true), 0);
			TestEqual(TEXT("outside -> full 100 at risk"),          UEmergeSafeZone::ValueAtRisk(100, false), 100);
			TestEqual(TEXT("outside but empty -> 0"),               UEmergeSafeZone::ValueAtRisk(0, false), 0);
			TestEqual(TEXT("negative carried clamps to 0"),         UEmergeSafeZone::ValueAtRisk(-50, false), 0);
		});
	});
}
#endif
