// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: traversal hop launch math.
// A non-ALS AI character (the zombie) clears a low obstacle with a parabolic LaunchCharacter hop:
// vertical velocity to top the obstacle with margin, forward velocity carried through.
// CONTRACT — CREATE ONE NEW class UEmergeHop (UBlueprintFunctionLibrary, EMERGECORE_API) in
//   NEW files Public/Nav/EmergeHop.h + Private/Nav/EmergeHop.cpp:
//   static float HopVerticalVelocity(float ObstacleHeightUu, float GravityZ, float ClearanceMarginUu);
//     // const float H = FMath::Max(ObstacleHeightUu, 0.0f) + FMath::Max(ClearanceMarginUu, 0.0f);
//     // const float G = FMath::Abs(GravityZ);
//     // if (G <= 0.0f || H <= 0.0f) { return 0.0f; }
//     // return FMath::Sqrt(2.0f * G * H);
//   static bool ShouldHop(float LowTraceHitDist, float TriggerDist, bool bHighTraceClear, bool bOnGround);
//     // return bOnGround && bHighTraceClear && LowTraceHitDist >= 0.0f && LowTraceHitDist <= TriggerDist;
// Deterministic, exact golden values, boundaries covered. Every function body wrapped in braces;
// every code path must return a value.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Nav/EmergeHop.h"

BEGIN_DEFINE_SPEC(FEmergeHopSpec, "Emergence.Nav.Hop",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeHopSpec)
void FEmergeHopSpec::Define()
{
	Describe("HopVerticalVelocity (v = sqrt(2*g*(h+margin)))", [this]()
	{
		It("matches exact golden values", [this]()
		{
			TestEqual(TEXT("h 80 + margin 20 at g 980 -> sqrt(2*980*100)=442.7188"),
				UEmergeHop::HopVerticalVelocity(80.f, -980.f, 20.f), 442.7188f, 1.e-2f);
			TestEqual(TEXT("h 50 + margin 0 at g 980 -> sqrt(98000)=313.0495"),
				UEmergeHop::HopVerticalVelocity(50.f, 980.f, 0.f), 313.0495f, 1.e-2f);
			TestEqual(TEXT("zero height+margin -> 0"), UEmergeHop::HopVerticalVelocity(0.f, -980.f, 0.f), 0.f);
			TestEqual(TEXT("zero gravity -> 0"), UEmergeHop::HopVerticalVelocity(100.f, 0.f, 20.f), 0.f);
			TestEqual(TEXT("negative height clamps to margin only"),
				UEmergeHop::HopVerticalVelocity(-50.f, -980.f, 20.f), 198.0f, 0.5f);
		});
	});
	Describe("ShouldHop (grounded + low blocked within trigger + high clear)", [this]()
	{
		It("matches exact golden values", [this]()
		{
			TestEqual(TEXT("hit at 100 within 150, high clear, grounded -> true"), UEmergeHop::ShouldHop(100.f, 150.f, true, true), true);
			TestEqual(TEXT("hit beyond trigger -> false"), UEmergeHop::ShouldHop(200.f, 150.f, true, true), false);
			TestEqual(TEXT("high blocked (wall not vaultable) -> false"), UEmergeHop::ShouldHop(100.f, 150.f, false, true), false);
			TestEqual(TEXT("airborne -> false"), UEmergeHop::ShouldHop(100.f, 150.f, true, false), false);
			TestEqual(TEXT("no hit (-1) -> false"), UEmergeHop::ShouldHop(-1.f, 150.f, true, true), false);
			TestEqual(TEXT("hit at exactly trigger -> true"), UEmergeHop::ShouldHop(150.f, 150.f, true, true), true);
		});
	});
}
#endif

