// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: influence-map math (threat/friendly
// fields for the sensor world-model). Research: sources stamp distance-falloff influence; layers combine.
// CONTRACT — NEW class UEmergeInfluence : public UBlueprintFunctionLibrary
//   in Public/Sensor/EmergeInfluence.h + Private/Sensor/EmergeInfluence.cpp, static UFUNCTIONs:
//   static float StampFalloff(float Strength, float Dist, float MaxDist, int32 CurvePow);
//     // if (MaxDist <= 0.0f || Dist >= MaxDist) return 0.0f;
//     // const float T = FMath::Clamp(Dist / MaxDist, 0.0f, 1.0f);
//     // return Strength * (1.0f - FMath::Pow(T, (float)FMath::Max(1, CurvePow)));
//   static float Tension(float Threat, float Friendly);      // return Threat + Friendly;
//   static float NetControl(float Friendly, float Threat);   // return Friendly - Threat;
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Sensor/EmergeInfluence.h"

BEGIN_DEFINE_SPEC(FEmergeInfluenceSpec, "Emergence.Sensor.Influence",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeInfluenceSpec)
void FEmergeInfluenceSpec::Define()
{
	Describe("StampFalloff", [this]()
	{
		It("linear + poly falloff, zero past max", [this]()
		{
			TestEqual(TEXT("at source -> full"), UEmergeInfluence::StampFalloff(100.f, 0.f, 200.f, 1), 100.f);
			TestEqual(TEXT("half dist linear -> 50"), UEmergeInfluence::StampFalloff(100.f, 100.f, 200.f, 1), 50.f);
			TestEqual(TEXT("at max -> 0"), UEmergeInfluence::StampFalloff(100.f, 200.f, 200.f, 1), 0.f);
			TestEqual(TEXT("half dist poly2 -> 75"), UEmergeInfluence::StampFalloff(100.f, 100.f, 200.f, 2), 75.f);
		});
	});
	Describe("Tension / NetControl", [this]()
	{
		It("matches exact golden values", [this]()
		{
			TestEqual(TEXT("tension 30+20 -> 50"), UEmergeInfluence::Tension(30.f, 20.f), 50.f);
			TestEqual(TEXT("net 20-30 -> -10"), UEmergeInfluence::NetControl(20.f, 30.f), -10.f);
		});
	});
}
#endif
