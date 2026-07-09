// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: lead a moving target so a chaser intercepts
// rather than tail-chases. CONTRACT — NEW class UEmergeIntercept : public UBlueprintFunctionLibrary
//   in Public/Nav/EmergeIntercept.h + Private/Nav/EmergeIntercept.cpp, static UFUNCTIONs:
//   static FVector PredictPosition(FVector TargetPos, FVector TargetVel, float LeadTime);
//     // return TargetPos + TargetVel * FMath::Max(0.0f, LeadTime);
//   static float LeadTime(float Distance, float PursuerSpeed);
//     // return (PursuerSpeed <= 1.0f) ? 0.0f : Distance / PursuerSpeed;
// Deterministic; cast FVector components to float in TestEqual.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Nav/EmergeIntercept.h"

BEGIN_DEFINE_SPEC(FEmergeInterceptSpec, "Emergence.Nav.Intercept",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeInterceptSpec)
void FEmergeInterceptSpec::Define()
{
	Describe("PredictPosition / LeadTime", [this]()
	{
		It("matches exact golden values", [this]()
		{
			const FVector P = UEmergeIntercept::PredictPosition(FVector(0,0,0), FVector(100,0,0), 2.f);
			TestEqual(TEXT("pos + vel*t"), (float)P.X, 200.0f);
			TestEqual(TEXT("leadtime dist/speed"), UEmergeIntercept::LeadTime(1000.f, 500.f), 2.0f);
			TestEqual(TEXT("not moving -> 0"), UEmergeIntercept::LeadTime(1000.f, 0.5f), 0.0f);
		});
	});
}
#endif
