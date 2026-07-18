#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"

#if WITH_AUTOMATION_TESTS

#include "UObject/Package.h"
#include "World/SafeZoneArbiter.h"

// FEATURE UNDER TEST: Deterministic owner assignment for loot in overlapping safe zones.
//
// FORMULA / ALGORITHM (exact):
//   Given an item inside N overlapping zones, the owning zone is the one with the
//   SMALLEST RegistrationTimestamp (earliest registered wins). Ties are impossible
//   because timestamps are unique monotonically increasing integers.
//     Owner = argmin_over_zones( Zone.RegistrationTimestamp )
//   The item's bSecured state, once resolved, MUST remain constant (true) and the
//   OwnerZoneId MUST remain constant across every tick (no flicker).
//
// SETUP (deterministic, no time / no RNG):
//   ZoneA: Id=10, Timestamp=100  (registered first -> should own)
//   ZoneB: Id=20, Timestamp=200
//   Item placed at intersection point.

BEGIN_DEFINE_SPEC(FImproveWorldSafeZoneSpec, "Emergence.World.ImproveWorldSafeZone",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
	USafeZoneArbiter* Arbiter;
END_DEFINE_SPEC(FImproveWorldSafeZoneSpec)

void FImproveWorldSafeZoneSpec::Define()
{
	BeforeEach([this]()
	{
		Arbiter = NewObject<USafeZoneArbiter>(GetTransientPackage());
		Arbiter->RegisterZone(/*ZoneId*/10, /*RegistrationTimestamp*/100);
		Arbiter->RegisterZone(/*ZoneId*/20, /*RegistrationTimestamp*/200);
		// Item 1 sits inside both zone 10 and zone 20 (the intersection).
		Arbiter->PlaceItem(/*ItemId*/1, /*OverlappingZoneIds*/{10, 20});
	});

	It("assigns the earliest-registered zone as the deterministic owner", [this]()
	{
		TestEqual(TEXT("OwnerZoneId (earliest timestamp wins)"), Arbiter->GetOwnerZoneId(1), 10);
	});

	It("marks the intersected item as secured", [this]()
	{
		TestTrue(TEXT("Item secured"), Arbiter->IsItemSecured(1));
	});

	It("keeps owner and secured state stable across 100 consecutive ticks", [this]()
	{
		for (int32 Tick = 0; Tick < 100; ++Tick)
		{
			Arbiter->Tick();
			TestEqual(FString::Printf(TEXT("OwnerZoneId at tick %d"), Tick), Arbiter->GetOwnerZoneId(1), 10);
			TestTrue(FString::Printf(TEXT("Secured at tick %d"), Tick), Arbiter->IsItemSecured(1));
		}
	});

	It("handles boundary: reversed registration order still picks earliest timestamp", [this]()
	{
		// Zone 5 registered later (ts=300) than existing zones; item only in 20 & 5 -> 20 owns.
		Arbiter->RegisterZone(/*ZoneId*/5, /*RegistrationTimestamp*/300);
		Arbiter->PlaceItem(/*ItemId*/2, /*OverlappingZoneIds*/{20, 5});
		TestEqual(TEXT("OwnerZoneId min-timestamp among {200,300}"), Arbiter->GetOwnerZoneId(2), 20);
	});

	It("handles boundary: single-zone item owns that zone", [this]()
	{
		Arbiter->PlaceItem(/*ItemId*/3, /*OverlappingZoneIds*/{20});
		TestEqual(TEXT("Single-zone owner"), Arbiter->GetOwnerZoneId(3), 20);
	});

	It("handles boundary: item in zero zones has no owner and is unsecured", [this]()
	{
		Arbiter->PlaceItem(/*ItemId*/4, /*OverlappingZoneIds*/{});
		TestEqual(TEXT("No owner sentinel"), Arbiter->GetOwnerZoneId(4), 0);
		TestFalse(TEXT("Unsecured when in no zone"), Arbiter->IsItemSecured(4));
	});

	AfterEach([this]()
	{
		Arbiter = nullptr;
	});
}

#endif // WITH_AUTOMATION_TESTS