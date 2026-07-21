// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: reticle style view-model (shell).
// SHELL stratum: picks the reticle shape and traits for the equipped optic - iron post, red dot, holo circle,
// or mil-dot scope (with rangefinding). Pairs with EmergeAdsZoom. Pure view config; deterministic.
// CONTRACT — NEW class UEmergeReticle : public UBlueprintFunctionLibrary
//   in Public/Shell/EmergeReticle.h + Private/Shell/EmergeReticle.cpp, static UFUNCTIONs.
//   OpticType int32: 0 IronSight, 1 RedDot, 2 Holo, 3 Scope. ReticleShape mirrors OpticType (0 post,1 dot,2 circle,3 mildot).
//
//   static int32 ReticleForOptic(int32 OpticType);
//     // switch: 0 -> 0; 1 -> 1; 2 -> 2; 3 -> 3; default -> 0;
//   static bool  HasRangefinder(int32 OpticType);   // return OpticType == 3;   // mil-dot scope
//   static float CenterDotSize(int32 OpticType);
//     // switch: 1 -> 2.0f; 2 -> 1.5f; default -> 1.0f;
//
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Shell/EmergeReticle.h"

BEGIN_DEFINE_SPEC(FEmergeReticleSpec, "Emergence.Shell.Reticle",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeReticleSpec)
void FEmergeReticleSpec::Define()
{
	Describe("Reticle selection", [this]()
	{
		It("maps optic to reticle, rangefinder, and dot size", [this]()
		{
			TestEqual(TEXT("ironsight post"), UEmergeReticle::ReticleForOptic(0), 0);
			TestEqual(TEXT("reddot dot"), UEmergeReticle::ReticleForOptic(1), 1);
			TestEqual(TEXT("holo circle"), UEmergeReticle::ReticleForOptic(2), 2);
			TestEqual(TEXT("scope mildot"), UEmergeReticle::ReticleForOptic(3), 3);
			TestEqual(TEXT("unknown -> post"), UEmergeReticle::ReticleForOptic(9), 0);
			TestTrue(TEXT("scope has rangefinder"), UEmergeReticle::HasRangefinder(3));
			TestFalse(TEXT("reddot no rangefinder"), UEmergeReticle::HasRangefinder(1));
			TestEqual(TEXT("reddot dot size 2.0"), UEmergeReticle::CenterDotSize(1), 2.0f);
			TestEqual(TEXT("holo dot size 1.5"), UEmergeReticle::CenterDotSize(2), 1.5f);
			TestEqual(TEXT("ironsight 1.0"), UEmergeReticle::CenterDotSize(0), 1.0f);
		});
	});
}
#endif
