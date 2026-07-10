// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: stealth detection multiplier stack
// (research: Blacklist gates + TLoU fill — stance x light x motion; light floored, NEVER a gate).
// CONTRACT — NEW class UEmergeStealthFill : public UBlueprintFunctionLibrary
//   in Public/Nav/EmergeStealthFill.h + Private/Nav/EmergeStealthFill.cpp, static UFUNCTIONs:
//   static float StanceMultiplier(int32 Stance);   // 0 stand->1.0, 1 crouch->0.6, 2 prone->0.35, else 1.0
//   static float LightMultiplier(float Light01);   // = FMath::Clamp(Light01, 0.2f, 1.f)
//   static float MotionMultiplier(float Speed01);  // = 1.f + FMath::Clamp(Speed01, 0.f, 1.f)
//   static float FillMultiplier(int32 Stance, float Light01, float Speed01);  // product of the three
//   static bool PassesVisibilityGate(float VisibleFraction);   // = VisibleFraction >= 0.25f
// Deterministic, exact golden values.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Nav/EmergeStealthFill.h"

BEGIN_DEFINE_SPEC(FEmergeStealthFillSpec, "Emergence.Nav.StealthFill",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeStealthFillSpec)
void FEmergeStealthFillSpec::Define()
{
	Describe("multipliers", [this]()
	{
		It("stance values", [this]()
		{
			TestEqual(TEXT("stand"), UEmergeStealthFill::StanceMultiplier(0), 1.f);
			TestEqual(TEXT("crouch"), UEmergeStealthFill::StanceMultiplier(1), 0.6f);
			TestEqual(TEXT("prone"), UEmergeStealthFill::StanceMultiplier(2), 0.35f);
			TestEqual(TEXT("unknown"), UEmergeStealthFill::StanceMultiplier(9), 1.f);
		});
		It("light floors at 0.2", [this]()
		{
			TestEqual(TEXT("0 -> 0.2"), UEmergeStealthFill::LightMultiplier(0.f), 0.2f);
			TestEqual(TEXT("0.6"), UEmergeStealthFill::LightMultiplier(0.6f), 0.6f);
			TestEqual(TEXT("1.5 -> 1"), UEmergeStealthFill::LightMultiplier(1.5f), 1.f);
		});
		It("motion 1..2", [this]()
		{
			TestEqual(TEXT("still"), UEmergeStealthFill::MotionMultiplier(0.f), 1.f);
			TestEqual(TEXT("sprint"), UEmergeStealthFill::MotionMultiplier(1.f), 2.f);
			TestEqual(TEXT("over"), UEmergeStealthFill::MotionMultiplier(1.4f), 2.f);
		});
	});
	Describe("product and gate", [this]()
	{
		It("multiplies the three", [this]()
		{
			TestNearlyEqual(TEXT("crouch dark still 0.12"), UEmergeStealthFill::FillMultiplier(1, 0.f, 0.f), 0.12f, 1e-4f);
			TestNearlyEqual(TEXT("stand lit sprint 2.0"), UEmergeStealthFill::FillMultiplier(0, 1.f, 1.f), 2.f, 1e-4f);
		});
		It("gate at 0.25 visible", [this]()
		{
			TestFalse(TEXT("0.24"), UEmergeStealthFill::PassesVisibilityGate(0.24f));
			TestTrue(TEXT("0.25"), UEmergeStealthFill::PassesVisibilityGate(0.25f));
		});
	});
}
#endif
