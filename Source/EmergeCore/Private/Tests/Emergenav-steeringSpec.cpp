// HUMAN-AUTHORED trusted acceptance spec (do not edit). NEW class UEmergeNavSteering : public UBlueprintFunctionLibrary.
// static FVector2D DesiredHeading(FVector2D From, FVector2D To); { const FVector2D D=To-From; return D.IsNearlyZero()?FVector2D::ZeroVector:D.GetSafeNormal(); }
// static float TurnErrorDeg(FVector2D Heading, FVector2D Facing); { if(Heading.IsNearlyZero()||Facing.IsNearlyZero()) return 0.0f; const float Dot=FMath::Clamp((float)FVector2D::DotProduct(Heading.GetSafeNormal(),Facing.GetSafeNormal()),-1.0f,1.0f); return FMath::RadiansToDegrees(FMath::Acos(Dot)); }
// static bool ShouldSlowForTurn(float TurnErrorDeg, float ThresholdDeg); { return TurnErrorDeg > ThresholdDeg; }
// NOTE: FVector2D components are `double` in UE5 - cast to float in TestEqual to avoid ambiguous overload (C2666).
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Nav/EmergeNavSteering.h"
BEGIN_DEFINE_SPEC(FEmergeNavSteeringSpec, "Emergence.Nav.Steering", EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeNavSteeringSpec)
void FEmergeNavSteeringSpec::Define()
{
	Describe("steering", [this]()
	{
		It("heading + turn error + slow gate", [this]()
		{
			const FVector2D H = UEmergeNavSteering::DesiredHeading(FVector2D(0, 0), FVector2D(10, 0));
			TestEqual(TEXT("east x"), (float)H.X, 1.0f);
			TestEqual(TEXT("east y"), (float)H.Y, 0.0f);
			TestEqual(TEXT("aligned -> 0"), UEmergeNavSteering::TurnErrorDeg(FVector2D(1, 0), FVector2D(1, 0)), 0.0f);
			TestEqual(TEXT("perp -> 90"), UEmergeNavSteering::TurnErrorDeg(FVector2D(1, 0), FVector2D(0, 1)), 90.0f, 0.01f);
			TestTrue(TEXT("100 > 90"), UEmergeNavSteering::ShouldSlowForTurn(100.f, 90.f));
			TestFalse(TEXT("45 < 90"), UEmergeNavSteering::ShouldSlowForTurn(45.f, 90.f));
		});
	});
}
#endif
