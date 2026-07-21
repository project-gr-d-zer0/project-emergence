// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: stance indicator view-model (shell).
// SHELL stratum: the stand/crouch/prone indicator and the movement + camera-height each stance implies (drives
// the HUD stance icon, the eye-height for the camera, and the move-speed the pawn should use). Pairs with
// EmergeSpreadStance (accuracy) and EmergeFracture (mobility). Pure view/config math; deterministic.
// CONTRACT — NEW class UEmergeStanceIndicator : public UBlueprintFunctionLibrary
//   in Public/Shell/EmergeStanceIndicator.h + Private/Shell/EmergeStanceIndicator.cpp, static UFUNCTIONs.
//   Stance int32: 0 Standing, 1 Crouched, 2 Prone.
//
//   static float EyeHeightCm(int32 Stance);
//     // switch: 0 -> 170.0f; 1 -> 100.0f; 2 -> 30.0f; default -> 170.0f;
//   static float MoveSpeedMultiplier(int32 Stance);
//     // switch: 0 -> 1.0f; 1 -> 0.6f; 2 -> 0.25f; default -> 1.0f;
//
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Shell/EmergeStanceIndicator.h"

BEGIN_DEFINE_SPEC(FEmergeStanceIndicatorSpec, "Emergence.Shell.StanceIndicator",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeStanceIndicatorSpec)
void FEmergeStanceIndicatorSpec::Define()
{
	Describe("EyeHeightCm", [this]()
	{
		It("camera height per stance", [this]()
		{
			TestEqual(TEXT("standing 170"), UEmergeStanceIndicator::EyeHeightCm(0), 170.0f);
			TestEqual(TEXT("crouched 100"), UEmergeStanceIndicator::EyeHeightCm(1), 100.0f);
			TestEqual(TEXT("prone 30"), UEmergeStanceIndicator::EyeHeightCm(2), 30.0f);
			TestEqual(TEXT("unknown -> standing"), UEmergeStanceIndicator::EyeHeightCm(9), 170.0f);
		});
	});

	Describe("MoveSpeedMultiplier", [this]()
	{
		It("lower stances move slower", [this]()
		{
			TestEqual(TEXT("standing 1.0"), UEmergeStanceIndicator::MoveSpeedMultiplier(0), 1.0f);
			TestEqual(TEXT("crouched 0.6"), UEmergeStanceIndicator::MoveSpeedMultiplier(1), 0.6f);
			TestEqual(TEXT("prone 0.25"), UEmergeStanceIndicator::MoveSpeedMultiplier(2), 0.25f);
		});
	});
}
#endif
