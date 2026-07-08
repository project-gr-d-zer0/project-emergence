// HUMAN-AUTHORED trusted acceptance spec. Feature: data-driven tuning structs.
// Contract: default-constructed tuning MUST reproduce the golden constants exactly (legacy statics
// delegate to the tuned versions), and custom tuning must deterministically shift behavior.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Combat/EmergeStagger.h"
#include "Combat/EmergeBallistics.h"

BEGIN_DEFINE_SPEC(FEmergeTuningSpec, "Emergence.Tuning.Structs",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeTuningSpec)
void FEmergeTuningSpec::Define()
{
	Describe("Default tuning == golden constants", [this]()
	{
		It("stagger tuned defaults match the legacy golden statics", [this]()
		{
			const FEmergeStaggerTuning D;
			TestEqual(TEXT("head/front 100 -> 150"),
				UEmergeStagger::HitStaggerAddTuned(D, 100.f, EEmergeBodyPart::Head, EEmergeHitDir::Front), 150.f);
			TestEqual(TEXT("legacy == tuned(default)"),
				UEmergeStagger::HitStaggerAdd(100.f, EEmergeBodyPart::Leg, EEmergeHitDir::Behind),
				UEmergeStagger::HitStaggerAddTuned(D, 100.f, EEmergeBodyPart::Leg, EEmergeHitDir::Behind));
			TestEqual(TEXT("meter 100 -> Stagger"),
				(int32)UEmergeStagger::StateForMeterTuned(D, 100.f), (int32)EEmergeStaggerState::Stagger);
			TestEqual(TEXT("crawl speed 0.35"),
				UEmergeStagger::LocomotionSpeedMultiplierTuned(D, EEmergeStaggerState::Crawl), 0.35f);
		});

		It("ballistics tuned defaults match the legacy golden statics", [this]()
		{
			const FEmergeBallisticsTuning D;
			TestTrue(TEXT("pen 30 vs class 2"), UEmergeBallistics::PenetratesTuned(D, 30.f, 2));
			TestFalse(TEXT("pen 19 vs class 2"), UEmergeBallistics::PenetratesTuned(D, 19.f, 2));
			TestEqual(TEXT("stopped 100/80 -> 15"), UEmergeBallistics::MitigatedDamageTuned(D, 100.f, 80.f, false), 15.f);
			TestEqual(TEXT("pen 100/80 -> 60"), UEmergeBallistics::MitigatedDamageTuned(D, 100.f, 80.f, true), 60.f);
		});
	});

	Describe("Custom tuning deterministically shifts behavior", [this]()
	{
		It("raised head weight and moved thresholds take effect", [this]()
		{
			FEmergeStaggerTuning T;
			T.HeadWeight = 2.0f;
			TestEqual(TEXT("head/front 100 -> 200 with weight 2"),
				UEmergeStagger::HitStaggerAddTuned(T, 100.f, EEmergeBodyPart::Head, EEmergeHitDir::Front), 200.f);

			T.StumbleThreshold = 50.0f;
			TestEqual(TEXT("meter 45 stays Moving with threshold 50"),
				(int32)UEmergeStagger::StateForMeterTuned(T, 45.f), (int32)EEmergeStaggerState::Moving);

			FEmergeBallisticsTuning B;
			B.BleedThroughFrac = 0.3f;
			TestEqual(TEXT("stopped 100 -> 30 with 30% bleed"),
				UEmergeBallistics::MitigatedDamageTuned(B, 100.f, 80.f, false), 30.f);
		});
	});
}
#endif
