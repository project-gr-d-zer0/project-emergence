// HUMAN-AUTHORED trusted acceptance spec (do not edit). R5 PROPERTY / METAMORPHIC tests.
// Golden-value specs pin exact points; these assert INVARIANTS that must hold across a whole RANGE of inputs -
// catching regressions and emergent/composition bugs that fixed goldens miss. Pure over the already-implemented
// survival functions; deterministic (grid sweep, no RNG). No new /core class - this file only #includes existing
// headers and checks their properties, so it joins the gate suite with no implementation.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Survival/EmergeDisease.h"
#include "Survival/EmergeMetabolism.h"
#include "Survival/EmergeBleeding.h"
#include "Shell/EmergeVitalHud.h"

BEGIN_DEFINE_SPEC(FEmergeSurvivalInvariantsSpec, "Emergence.Survival.Invariants",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeSurvivalInvariantsSpec)
void FEmergeSurvivalInvariantsSpec::Define()
{
	Describe("Immunity01", [this]()
	{
		It("stays in [0,1] and is monotonic non-decreasing in each vital", [this]()
		{
			for (int32 e = 0; e <= 5000; e += 500)
			for (int32 w = 0; w <= 5000; w += 1250)
			for (int32 h = 0; h <= 10; ++h)
			{
				const float H = h / 10.0f;
				const float imm = UEmergeDisease::Immunity01((float)e, (float)w, H, 0.5f);
				TestTrue(TEXT("immunity in [0,1]"), imm >= 0.0f && imm <= 1.0f);
			}
			// monotonic in energy (others fixed)
			float prev = -1.0f;
			for (int32 e = 0; e <= 5000; e += 250)
			{
				const float imm = UEmergeDisease::Immunity01((float)e, 2500.f, 0.5f, 0.5f);
				TestTrue(TEXT("non-decreasing in energy"), imm >= prev - 1e-4f);
				prev = imm;
			}
		});
	});

	Describe("Bleeding & bleeding-pressure", [this]()
	{
		It("blood-pressure scale in [0.3,1]; source loss is never positive", [this]()
		{
			for (int32 b = 0; b <= 6000; b += 250)
			{
				const float s = UEmergeBleeding::BloodPressureScale((float)b);
				TestTrue(TEXT("scale in [0.3,1]"), s >= 0.3f - 1e-4f && s <= 1.0f + 1e-4f);
				for (int32 intensity = 0; intensity <= 2; ++intensity)
				{
					const float loss = UEmergeBleeding::SourceBloodLossPerTick(intensity, (float)b, 1.0f);
					TestTrue(TEXT("bleeding never adds blood"), loss <= 0.0f + 1e-4f);
				}
			}
		});
	});

	Describe("Metabolism & disease bounds", [this]()
	{
		It("drain is non-negative; agent count stays in [0,max]", [this]()
		{
			for (int32 mv = 0; mv <= 3; ++mv)
			for (int32 s = 0; s <= 5000; s += 1000)
			{
				const float d = UEmergeMetabolism::DrainPerTick(mv, (float)s, 1.0f);
				TestTrue(TEXT("drain >= 0"), d >= 0.0f);
			}
			for (int32 c = 0; c <= 300; c += 25)
			for (int32 pot = 0; pot <= 4; ++pot)
			for (int32 imm = 0; imm <= 4; ++imm)
			{
				const float delta = UEmergeDisease::GrowDelta(0.75f, 1.0f, pot, imm, 1.0f);
				const float next = UEmergeDisease::StepCount((float)c, delta, 300.0f);
				TestTrue(TEXT("agent count in [0,max]"), next >= 0.0f && next <= 300.0f);
			}
		});
	});

	Describe("HUD view-model bounds", [this]()
	{
		It("bar fill in [0,1]; every stat level in [0,4]", [this]()
		{
			for (int32 v = 0; v <= 5000; v += 250)
			{
				const float f = UEmergeVitalHud::BarFill01((float)v, 5000.f);
				TestTrue(TEXT("fill in [0,1]"), f >= 0.0f && f <= 1.0f);
				const int32 lvl = UEmergeVitalHud::EnergyStatLevel((float)v);
				TestTrue(TEXT("stat level in [0,4]"), lvl >= 0 && lvl <= 4);
			}
		});
	});
}
#endif
