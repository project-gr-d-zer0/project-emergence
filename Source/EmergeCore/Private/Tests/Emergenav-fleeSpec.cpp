// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: evasion math (player/AI fleeing a threat).
// CONTRACT — NEW class UEmergeFlee : public UBlueprintFunctionLibrary
//   in Public/Nav/EmergeFlee.h + Private/Nav/EmergeFlee.cpp, static UFUNCTIONs:
//   static FVector FleeDirection(FVector Self, FVector Threat);
//     // const FVector D = Self - Threat; return D.IsNearlyZero() ? FVector::ZeroVector : D.GetSafeNormal();
//   static FVector FleePoint(FVector Self, FVector Threat, float Distance);
//     // return Self + FleeDirection(Self, Threat) * Distance;
//   static float ThreatLevel(float Dist, float DangerRadius);
//     // if (DangerRadius <= 0.0f) return 0.0f; return FMath::Clamp(1.0f - Dist / DangerRadius, 0.0f, 1.0f);
// Deterministic, exact golden values, boundaries covered.
// NOTE: FVector components are double -> cast (float) in TestEqual.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Nav/EmergeFlee.h"

BEGIN_DEFINE_SPEC(FEmergeFleeSpec, "Emergence.Nav.Flee",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeFleeSpec)
void FEmergeFleeSpec::Define()
{
	Describe("FleeDirection (unit, away from threat)", [this]()
	{
		It("matches exact golden values", [this]()
		{
			const FVector D = UEmergeFlee::FleeDirection(FVector(10, 0, 0), FVector(0, 0, 0));
			TestEqual(TEXT("east away x"), (float)D.X, 1.0f);
			TestEqual(TEXT("east away y"), (float)D.Y, 0.0f);
			const FVector Z = UEmergeFlee::FleeDirection(FVector(5, 5, 0), FVector(5, 5, 0));
			TestTrue(TEXT("coincident -> zero"), Z.IsNearlyZero());
		});
	});
	Describe("FleePoint", [this]()
	{
		It("self offset away by distance", [this]()
		{
			const FVector P = UEmergeFlee::FleePoint(FVector(10, 0, 0), FVector(0, 0, 0), 100.f);
			TestEqual(TEXT("x = 110"), (float)P.X, 110.0f);
		});
	});
	Describe("ThreatLevel", [this]()
	{
		It("1 at contact, 0 at/after danger radius", [this]()
		{
			TestEqual(TEXT("contact -> 1"), UEmergeFlee::ThreatLevel(0.f, 500.f), 1.0f);
			TestEqual(TEXT("half -> 0.5"), UEmergeFlee::ThreatLevel(250.f, 500.f), 0.5f);
			TestEqual(TEXT("edge -> 0"), UEmergeFlee::ThreatLevel(500.f, 500.f), 0.0f);
		});
	});
}
#endif
