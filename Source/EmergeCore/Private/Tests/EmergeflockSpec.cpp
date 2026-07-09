// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: boid herd steering (Reynolds) so the horde
// moves as a coherent mass. Pure vector math; runs in a Mass processor or an Actor later.
// CONTRACT — NEW class UEmergeFlock : public UBlueprintFunctionLibrary
//   in Public/Nav/EmergeFlock.h + Private/Nav/EmergeFlock.cpp, static UFUNCTIONs:
//   static FVector Separation(FVector Self, FVector Neighbor, float DesiredDist);
//     // const FVector D = Self - Neighbor; const float Dist = D.Size();
//     // if (Dist <= 0.0f || Dist >= DesiredDist) return FVector::ZeroVector;
//     // return D.GetSafeNormal() * (1.0f - Dist / DesiredDist);   // stronger the closer they are
//   static FVector Cohesion(FVector Self, FVector GroupCenter);
//     // const FVector D = GroupCenter - Self; return D.IsNearlyZero() ? FVector::ZeroVector : D.GetSafeNormal();
//   static FVector Alignment(FVector AvgHeading);
//     // return AvgHeading.IsNearlyZero() ? FVector::ZeroVector : AvgHeading.GetSafeNormal();
//   static FVector Combine(FVector Sep, FVector Coh, FVector Ali, FVector Seek, float WSep, float WCoh, float WAli, float WSeek);
//     // const FVector Sum = Sep*WSep + Coh*WCoh + Ali*WAli + Seek*WSeek;
//     // return Sum.IsNearlyZero() ? FVector::ZeroVector : Sum.GetSafeNormal();
// Deterministic; cast FVector components to float in TestEqual.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Nav/EmergeFlock.h"

BEGIN_DEFINE_SPEC(FEmergeFlockSpec, "Emergence.Nav.Flock",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeFlockSpec)
void FEmergeFlockSpec::Define()
{
	Describe("Separation (push away, stronger when closer)", [this]()
	{
		It("matches exact golden values", [this]()
		{
			const FVector S = UEmergeFlock::Separation(FVector(50,0,0), FVector(0,0,0), 100.f);
			TestEqual(TEXT("push +x half strength"), (float)S.X, 0.5f);
			TestTrue(TEXT("beyond desired -> zero"), UEmergeFlock::Separation(FVector(200,0,0), FVector(0,0,0), 100.f).IsNearlyZero());
		});
	});
	Describe("Cohesion / Combine", [this]()
	{
		It("matches exact golden values", [this]()
		{
			const FVector C = UEmergeFlock::Cohesion(FVector(0,0,0), FVector(10,0,0));
			TestEqual(TEXT("toward center +x"), (float)C.X, 1.0f);
			const FVector K = UEmergeFlock::Combine(FVector(1,0,0), FVector::ZeroVector, FVector::ZeroVector, FVector::ZeroVector, 1.f, 0.f, 0.f, 0.f);
			TestEqual(TEXT("combine sep only -> +x"), (float)K.X, 1.0f);
		});
	});
}
#endif
