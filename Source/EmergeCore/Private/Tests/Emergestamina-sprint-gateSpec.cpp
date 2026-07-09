// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: sprint-exhaustion gate with hysteresis.
// Sprint drains stamina (UEmergeStaminaComponent::Simulate already does the drain math); this gate
// decides when sprinting is DENIED: exhausted at 0, and stays denied until stamina recovers past a
// re-arm threshold (hysteresis prevents sprint-flapping right at 0).
// CONTRACT — CREATE ONE NEW class UEmergeSprintGate (UBlueprintFunctionLibrary, EMERGECORE_API) in
//   NEW files Public/Survival/EmergeSprintGate.h + Private/Survival/EmergeSprintGate.cpp:
//   static bool UpdateExhausted(float Stamina, bool bWasExhausted, float ReArmThreshold);
//     // if (Stamina <= 0.0f) { return true; }
//     // if (bWasExhausted && Stamina < FMath::Max(ReArmThreshold, 0.0f)) { return true; }
//     // return false;
// Deterministic, exact golden values, boundaries covered. Every function body wrapped in braces;
// every code path must return a value.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Survival/EmergeSprintGate.h"

BEGIN_DEFINE_SPEC(FEmergeSprintGateSpec, "Emergence.Survival.SprintGate",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeSprintGateSpec)
void FEmergeSprintGateSpec::Define()
{
	Describe("UpdateExhausted (0 trips it, re-arm threshold releases it)", [this]()
	{
		It("trips at zero and below", [this]()
		{
			TestEqual(TEXT("0 -> exhausted"), UEmergeSprintGate::UpdateExhausted(0.f, false, 20.f), true);
			TestEqual(TEXT("negative -> exhausted"), UEmergeSprintGate::UpdateExhausted(-5.f, true, 20.f), true);
		});
		It("holds exhausted until re-arm threshold", [this]()
		{
			TestEqual(TEXT("recovering 10 < 20 -> still exhausted"), UEmergeSprintGate::UpdateExhausted(10.f, true, 20.f), true);
			TestEqual(TEXT("at threshold 20 -> released"), UEmergeSprintGate::UpdateExhausted(20.f, true, 20.f), false);
			TestEqual(TEXT("past threshold 25 -> released"), UEmergeSprintGate::UpdateExhausted(25.f, true, 20.f), false);
		});
		It("does not trip early while draining", [this]()
		{
			TestEqual(TEXT("10 not-exhausted -> stays free"), UEmergeSprintGate::UpdateExhausted(10.f, false, 20.f), false);
			TestEqual(TEXT("100 -> free"), UEmergeSprintGate::UpdateExhausted(100.f, false, 20.f), false);
		});
		It("clamps a negative re-arm threshold to 0", [this]()
		{
			TestEqual(TEXT("exhausted, 5 stamina, rearm -10 -> released"), UEmergeSprintGate::UpdateExhausted(5.f, true, -10.f), false);
			TestEqual(TEXT("exhausted, 0 stamina, rearm -10 -> exhausted"), UEmergeSprintGate::UpdateExhausted(0.f, true, -10.f), true);
		});
	});
}
#endif

