// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: ballistic range dropoff (combat/gunplay).
// Damage falls off between a near and far range (full damage inside FalloffStart, floored to MinMultiplier past
// FalloffEnd, linear between); bullet velocity bleeds with distance via a simple linear drag. Distinct from the
// existing EmergeBallistics - this is the range-attenuation layer. TUNABLE per-weapon, fixed deterministic model.
// CONTRACT — NEW class UEmergeBallisticDropoff : public UBlueprintFunctionLibrary
//   in Public/Combat/EmergeBallisticDropoff.h + Private/Combat/EmergeBallisticDropoff.cpp, static UFUNCTIONs.
//
//   static float DamageAtRange(float BaseDamage, float Distance, float FalloffStart, float FalloffEnd, float MinMultiplier);
//     // if (Distance <= FalloffStart) return BaseDamage;
//     // if (Distance >= FalloffEnd)   return BaseDamage * MinMultiplier;
//     // const float t = (Distance - FalloffStart) / (FalloffEnd - FalloffStart);
//     // return BaseDamage * FMath::Lerp(1.0f, MinMultiplier, t);
//   static float VelocityAtRange(float MuzzleVel, float Distance, float DragPerMeter);
//     // return FMath::Max(0.0f, MuzzleVel - DragPerMeter * Distance);
//
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Combat/EmergeBallisticDropoff.h"

BEGIN_DEFINE_SPEC(FEmergeBallisticDropoffSpec, "Emergence.Combat.BallisticDropoff",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeBallisticDropoffSpec)
void FEmergeBallisticDropoffSpec::Define()
{
	Describe("DamageAtRange", [this]()
	{
		It("full inside near range, floored past far, linear between", [this]()
		{
			TestEqual(TEXT("point blank -> full"), UEmergeBallisticDropoff::DamageAtRange(100.f, 10.f, 20.f, 100.f, 0.5f), 100.0f);
			TestEqual(TEXT("at start -> full"), UEmergeBallisticDropoff::DamageAtRange(100.f, 20.f, 20.f, 100.f, 0.5f), 100.0f);
			TestEqual(TEXT("midway -> 75"), UEmergeBallisticDropoff::DamageAtRange(100.f, 60.f, 20.f, 100.f, 0.5f), 75.0f);
			TestEqual(TEXT("at end -> floor 50"), UEmergeBallisticDropoff::DamageAtRange(100.f, 100.f, 20.f, 100.f, 0.5f), 50.0f);
			TestEqual(TEXT("beyond -> floor 50"), UEmergeBallisticDropoff::DamageAtRange(100.f, 300.f, 20.f, 100.f, 0.5f), 50.0f);
		});
	});

	Describe("VelocityAtRange", [this]()
	{
		It("linear drag, never negative", [this]()
		{
			TestEqual(TEXT("muzzle"), UEmergeBallisticDropoff::VelocityAtRange(900.f, 0.f, 0.5f), 900.0f);
			TestEqual(TEXT("100m -> 850"), UEmergeBallisticDropoff::VelocityAtRange(900.f, 100.f, 0.5f), 850.0f);
			TestEqual(TEXT("far bleeds to 0"), UEmergeBallisticDropoff::VelocityAtRange(900.f, 2000.f, 0.5f), 0.0f);
		});
	});
}
#endif
