// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: the decaying-knowledge math for the shared
// influence grid (herd's belief of where the player is). Exponential temporal decay + cell diffusion (blur)
// + expanding search ring (radius = player speed * staleness) + dispersal check. THE horde-tracking core.
// CONTRACT — NEW class UEmergeInfluenceDecay : public UBlueprintFunctionLibrary
//   in Public/Nav/EmergeInfluenceDecay.h + Private/Nav/EmergeInfluenceDecay.cpp, static UFUNCTIONs:
//   static float DecayExp(float Value, float DtSeconds, float Tau);
//     // if (Tau <= 0.0f) return Value; return Value * FMath::Exp(-FMath::Max(0.0f, DtSeconds) / Tau);
//   static float DiffuseCell(float Center, float NeighborAvg, float Rate);
//     // return FMath::Lerp(Center, NeighborAvg, FMath::Clamp(Rate, 0.0f, 1.0f));   // spatial blur = search spreads
//   static float SearchRadius(float PlayerSpeed, float StalenessSeconds, float BaseRadius, float MaxRadius);
//     // return FMath::Min(MaxRadius, BaseRadius + FMath::Max(0.0f, PlayerSpeed) * FMath::Max(0.0f, StalenessSeconds));
//   static bool IsDispersed(float PeakConfidence, float Floor);   // return PeakConfidence < Floor;
// Deterministic; exp uses tolerance in tests.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Nav/EmergeInfluenceDecay.h"

BEGIN_DEFINE_SPEC(FEmergeInfluenceDecaySpec, "Emergence.Influence.Decay",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeInfluenceDecaySpec)
void FEmergeInfluenceDecaySpec::Define()
{
	Describe("DecayExp (exponential forgetting)", [this]()
	{
		It("matches e^-1 at one time-constant", [this]()
		{
			TestEqual(TEXT("no time -> unchanged"), UEmergeInfluenceDecay::DecayExp(1.f, 0.f, 2.f), 1.0f);
			TestEqual(TEXT("one tau -> 1/e"), UEmergeInfluenceDecay::DecayExp(1.f, 2.f, 2.f), 0.3679f, 0.001f);
			TestEqual(TEXT("tau<=0 -> unchanged"), UEmergeInfluenceDecay::DecayExp(1.f, 5.f, 0.f), 1.0f);
		});
	});
	Describe("DiffuseCell / SearchRadius / IsDispersed", [this]()
	{
		It("matches exact golden values", [this]()
		{
			TestEqual(TEXT("blur half toward neighbor"), UEmergeInfluenceDecay::DiffuseCell(0.f, 1.f, 0.5f), 0.5f);
			TestEqual(TEXT("ring = base + speed*staleness"), UEmergeInfluenceDecay::SearchRadius(600.f, 2.f, 100.f, 5000.f), 1300.f);
			TestEqual(TEXT("ring clamps to max"), UEmergeInfluenceDecay::SearchRadius(600.f, 100.f, 100.f, 5000.f), 5000.f);
			TestTrue(TEXT("0.05 < 0.1 -> dispersed"), UEmergeInfluenceDecay::IsDispersed(0.05f, 0.1f));
			TestFalse(TEXT("0.5 >= 0.1 -> not dispersed"), UEmergeInfluenceDecay::IsDispersed(0.5f, 0.1f));
		});
	});
}
#endif
