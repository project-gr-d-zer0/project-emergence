// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: persistent world-memory confidence
// decay (entities/places age out like a forgetting curve). Linear decay for exact determinism.
// CONTRACT — NEW class UEmergeMemory : public UBlueprintFunctionLibrary
//   in Public/Sensor/EmergeMemory.h + Private/Sensor/EmergeMemory.cpp, static UFUNCTIONs:
//   static float ConfidenceAfter(float LastConf, float DtSeconds, float DecaySeconds);
//     // return FMath::Clamp(LastConf - FMath::Max(0.0f, DtSeconds) / FMath::Max(0.001f, DecaySeconds), 0.0f, 1.0f);
//   static bool IsStale(float Confidence, float Floor);   // return Confidence < Floor;
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Sensor/EmergeMemory.h"

BEGIN_DEFINE_SPEC(FEmergeMemorySpec, "Emergence.Sensor.Memory",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeMemorySpec)
void FEmergeMemorySpec::Define()
{
	Describe("ConfidenceAfter (linear decay, clamped 0..1)", [this]()
	{
		It("matches exact golden values", [this]()
		{
			TestEqual(TEXT("no time -> unchanged"), UEmergeMemory::ConfidenceAfter(1.f, 0.f, 10.f), 1.f);
			TestEqual(TEXT("half decay -> 0.5"), UEmergeMemory::ConfidenceAfter(1.f, 5.f, 10.f), 0.5f);
			TestEqual(TEXT("full decay -> 0"), UEmergeMemory::ConfidenceAfter(1.f, 10.f, 10.f), 0.f);
			TestEqual(TEXT("over -> floors at 0"), UEmergeMemory::ConfidenceAfter(1.f, 30.f, 10.f), 0.f);
		});
	});
	Describe("IsStale", [this]()
	{
		It("stale below floor", [this]()
		{
			TestTrue(TEXT("0.1 < 0.2"), UEmergeMemory::IsStale(0.1f, 0.2f));
			TestFalse(TEXT("0.3 >= 0.2"), UEmergeMemory::IsStale(0.3f, 0.2f));
		});
	});
}
#endif
