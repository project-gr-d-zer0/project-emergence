// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: grenade blast radius (combat/gunplay).
// The explosion itself (distinct from EmergeThrowableTimer's fuse): damage falls off linearly from the blast
// center to zero at the blast radius, with a lethal inner ring. TUNABLE per-explosive, fixed deterministic model.
// CONTRACT — NEW class UEmergeGrenadeBlast : public UBlueprintFunctionLibrary
//   in Public/Combat/EmergeGrenadeBlast.h + Private/Combat/EmergeGrenadeBlast.cpp, static UFUNCTIONs.
//
//   static float DamageAtDistance(float MaxDamage, float Distance, float BlastRadius);
//     // if (Distance >= BlastRadius) return 0.0f;   return MaxDamage * (1.0f - Distance / BlastRadius);
//   static bool  InLethalRange(float Distance, float LethalRadius);   // return Distance <= LethalRadius;
//
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Combat/EmergeGrenadeBlast.h"

BEGIN_DEFINE_SPEC(FEmergeGrenadeBlastSpec, "Emergence.Combat.GrenadeBlast",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeGrenadeBlastSpec)
void FEmergeGrenadeBlastSpec::Define()
{
	Describe("DamageAtDistance", [this]()
	{
		It("linear falloff from center to blast radius", [this]()
		{
			TestEqual(TEXT("epicenter -> full"), UEmergeGrenadeBlast::DamageAtDistance(100.f, 0.f, 10.f), 100.0f);
			TestEqual(TEXT("half radius -> half"), UEmergeGrenadeBlast::DamageAtDistance(100.f, 5.f, 10.f), 50.0f);
			TestEqual(TEXT("at radius -> 0"), UEmergeGrenadeBlast::DamageAtDistance(100.f, 10.f, 10.f), 0.0f);
			TestEqual(TEXT("beyond -> 0"), UEmergeGrenadeBlast::DamageAtDistance(100.f, 20.f, 10.f), 0.0f);
		});
	});

	Describe("InLethalRange", [this]()
	{
		It("lethal inner ring is inclusive", [this]()
		{
			TestTrue(TEXT("3m inside 5m lethal"), UEmergeGrenadeBlast::InLethalRange(3.f, 5.f));
			TestTrue(TEXT("at 5m lethal edge"), UEmergeGrenadeBlast::InLethalRange(5.f, 5.f));
			TestFalse(TEXT("6m survivable"), UEmergeGrenadeBlast::InLethalRange(6.f, 5.f));
		});
	});
}
#endif
