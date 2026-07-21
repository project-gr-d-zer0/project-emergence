// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: world/target memory (zombie AI sensor). [CORPUS-FIX]
// SOURCE-CORRECTED to PZ: pz-decompiled-b41 IsoZombie.java memory is a DISCRETE forget-timeout, not a decay curve.
// TimeSinceSeenFlesh accumulates ~GameTime.getMultiplier() per update (L2102); when it exceeds one of the 4 sandbox
// memory tiers 1250/800/500/25 (High/Normal/Low/None), the zombie drops the target instantly (L2196). The prior
// continuous ConfidenceAfter/IsStale decay is kept (smooth awareness UI), but the ACTUAL forget behavior now
// matches PZ's tiered timeout.
// CONTRACT — class UEmergeMemory : public UBlueprintFunctionLibrary
//   in Public/Sensor/EmergeMemory.h + Private/Sensor/EmergeMemory.cpp, static UFUNCTIONs.
//   Keep existing: ConfidenceAfter(LastConf, DtSeconds, DecaySeconds) = Clamp(LastConf - DtSeconds/DecaySeconds, 0, 1);
//                  IsStale(Confidence, Floor) = Confidence < Floor.
//   ADD (PZ-grounded forget timeout). MemoryTier int32: 0 None, 1 Low, 2 Normal, 3 High.
//
//   static float ForgetThreshold(int32 MemoryTier);
//     // switch: 3 -> 1250.0f; 2 -> 800.0f; 1 -> 500.0f; default(0/None) -> 25.0f;   // IsoZombie sandbox tiers
//   static float AccumulateTimeSinceSeen(float Current, float UpdateMultiplier);   // return Current + UpdateMultiplier;
//   static bool  HasForgotten(float TimeSinceSeen, int32 MemoryTier);   // return TimeSinceSeen > ForgetThreshold(MemoryTier);
//
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Sensor/EmergeMemory.h"

BEGIN_DEFINE_SPEC(FEmergeMemorySpec, "Emergence.Sensor.Memory",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeMemorySpec)
void FEmergeMemorySpec::Define()
{
	Describe("ConfidenceAfter (linear decay, clamped 0..1) - kept", [this]()
	{
		It("smooth awareness decay", [this]()
		{
			TestEqual(TEXT("no time -> unchanged"), UEmergeMemory::ConfidenceAfter(1.f, 0.f, 10.f), 1.f);
			TestEqual(TEXT("half decay -> 0.5"), UEmergeMemory::ConfidenceAfter(1.f, 5.f, 10.f), 0.5f);
			TestEqual(TEXT("full decay -> 0"), UEmergeMemory::ConfidenceAfter(1.f, 10.f, 10.f), 0.f);
			TestTrue(TEXT("stale below floor"), UEmergeMemory::IsStale(0.05f, 0.1f));
			TestFalse(TEXT("not stale above floor"), UEmergeMemory::IsStale(0.5f, 0.1f));
		});
	});

	Describe("PZ forget timeout (IsoZombie tiers 1250/800/500/25)", [this]()
	{
		It("holds target until time-since-seen exceeds the tier, then drops instantly", [this]()
		{
			TestEqual(TEXT("High 1250"), UEmergeMemory::ForgetThreshold(3), 1250.0f);
			TestEqual(TEXT("Normal 800"), UEmergeMemory::ForgetThreshold(2), 800.0f);
			TestEqual(TEXT("Low 500"), UEmergeMemory::ForgetThreshold(1), 500.0f);
			TestEqual(TEXT("None 25"), UEmergeMemory::ForgetThreshold(0), 25.0f);
			TestEqual(TEXT("accumulate by update multiplier"), UEmergeMemory::AccumulateTimeSinceSeen(100.f, 1.5f), 101.5f);
			TestFalse(TEXT("within Normal tier -> still remembers"), UEmergeMemory::HasForgotten(799.f, 2));
			TestTrue(TEXT("past Normal tier -> forgotten"), UEmergeMemory::HasForgotten(801.f, 2));
			TestTrue(TEXT("None tier forgets fast (past 25)"), UEmergeMemory::HasForgotten(26.f, 0));
		});
	});
}
#endif
