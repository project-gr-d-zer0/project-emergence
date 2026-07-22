// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: deterministic stumble/stagger RULES core.
// The differentiator, scale-first (L4D-style deterministic discrete states; runs on the whole horde).
// CONTRACT to implement — NEW header Public/Combat/EmergeStagger.h + Private/Combat/EmergeStagger.cpp:
//   UENUM(BlueprintType) enum class EEmergeBodyPart : uint8 { Head, Torso, Arm, Leg };
//   UENUM(BlueprintType) enum class EEmergeHitDir  : uint8 { Front, Opposing, Behind };
//   UENUM(BlueprintType) enum class EEmergeStaggerState : uint8 { Moving, Stumble, Stagger, Knockdown, Crawl, Dead };
//   UCLASS() class UEmergeStagger : public UBlueprintFunctionLibrary { GENERATED_BODY() public:
//     static float HitStaggerAdd(float Damage, EEmergeBodyPart Part, EEmergeHitDir Dir);
//       // return Damage * partWeight * dirFactor
//       //   partWeight: Head 1.5, Torso 1.0, Leg 0.6, Arm 0.4 ; dirFactor: Front 1.0, Opposing 1.25, Behind 0.75
//     static EEmergeStaggerState StateForMeter(float Meter);
//       // Meter < 40 -> Moving; [40,80) -> Stumble; [80,120) -> Stagger; >=120 -> Knockdown
//     static float LocomotionSpeedMultiplier(EEmergeStaggerState State);
//       // Moving 1.0; Crawl 0.35; Stumble/Stagger/Knockdown/Dead 0.0
//   };
// Deterministic, exact golden values, boundaries covered. First-pass numbers; tune later via data assets.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Combat/EmergeStagger.h"

BEGIN_DEFINE_SPEC(FEmergeStaggerSpec, "Emergence.Combat.Stagger",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeStaggerSpec)
void FEmergeStaggerSpec::Define()
{
	Describe("HitStaggerAdd (Damage * partWeight * dirFactor)", [this]()
	{
		It("matches exact golden values across parts and directions", [this]()
		{
			TestEqual(TEXT("30 torso front -> 30"),    UEmergeStagger::HitStaggerAdd(30.f, EEmergeBodyPart::Torso, EEmergeHitDir::Front), 30.f);
			TestEqual(TEXT("30 torso opposing -> 37.5"),UEmergeStagger::HitStaggerAdd(30.f, EEmergeBodyPart::Torso, EEmergeHitDir::Opposing), 37.5f);
			TestEqual(TEXT("50 leg front -> 30"),       UEmergeStagger::HitStaggerAdd(50.f, EEmergeBodyPart::Leg, EEmergeHitDir::Front), 30.f);
			TestEqual(TEXT("20 head behind -> 22.5"),   UEmergeStagger::HitStaggerAdd(20.f, EEmergeBodyPart::Head, EEmergeHitDir::Behind), 22.5f);
			TestEqual(TEXT("10 arm front -> 4"),        UEmergeStagger::HitStaggerAdd(10.f, EEmergeBodyPart::Arm, EEmergeHitDir::Front), 4.f);
		});
	});

	Describe("StateForMeter (thresholds 40/80/120)", [this]()
	{
		It("maps each band and boundary to the exact state", [this]()
		{
			TestTrue(TEXT("0 -> Moving"),        UEmergeStagger::StateForMeter(0.f)   == EEmergeStaggerState::Moving);
			TestTrue(TEXT("39.9 -> Moving"),     UEmergeStagger::StateForMeter(39.9f) == EEmergeStaggerState::Moving);
			TestTrue(TEXT("40 -> Stumble"),      UEmergeStagger::StateForMeter(40.f)  == EEmergeStaggerState::Stumble);
			TestTrue(TEXT("79.9 -> Stumble"),    UEmergeStagger::StateForMeter(79.9f) == EEmergeStaggerState::Stumble);
			TestTrue(TEXT("80 -> Stagger"),      UEmergeStagger::StateForMeter(80.f)  == EEmergeStaggerState::Stagger);
			TestTrue(TEXT("119.9 -> Stagger"),   UEmergeStagger::StateForMeter(119.9f)== EEmergeStaggerState::Stagger);
			TestTrue(TEXT("120 -> Knockdown"),   UEmergeStagger::StateForMeter(120.f) == EEmergeStaggerState::Knockdown);
			TestTrue(TEXT("200 -> Knockdown"),   UEmergeStagger::StateForMeter(200.f) == EEmergeStaggerState::Knockdown);
		});
	});

	Describe("LocomotionSpeedMultiplier", [this]()
	{
		It("stops movement while stumbling, slows a crawl, full when moving", [this]()
		{
			TestEqual(TEXT("Moving -> 1.0"),     UEmergeStagger::LocomotionSpeedMultiplier(EEmergeStaggerState::Moving), 1.0f);
			TestEqual(TEXT("Crawl -> 0.35"),     UEmergeStagger::LocomotionSpeedMultiplier(EEmergeStaggerState::Crawl), 0.35f);
			TestEqual(TEXT("Stumble -> 0"),      UEmergeStagger::LocomotionSpeedMultiplier(EEmergeStaggerState::Stumble), 0.0f);
			TestEqual(TEXT("Stagger -> 0"),      UEmergeStagger::LocomotionSpeedMultiplier(EEmergeStaggerState::Stagger), 0.0f);
			TestEqual(TEXT("Knockdown -> 0"),    UEmergeStagger::LocomotionSpeedMultiplier(EEmergeStaggerState::Knockdown), 0.0f);
		});
	});
}
#endif
