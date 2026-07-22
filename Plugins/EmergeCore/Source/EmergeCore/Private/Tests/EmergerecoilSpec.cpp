// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: recoil pattern step (combat/gunplay).
// Tarkov-style per-shot recoil: vertical climb accumulates with each shot in a burst (capped), horizontal kick
// zig-zags left/right deterministically by shot index, and the muzzle recovers toward center between shots.
// These are TUNABLE design values (no global decompiled constant exists - DayZ recoil is per-weapon config), but
// the MODEL is fixed and deterministic so it can be gated. Pure math, no RNG.
// CONTRACT — NEW class UEmergeRecoil : public UBlueprintFunctionLibrary
//   in Public/Combat/EmergeRecoil.h + Private/Combat/EmergeRecoil.cpp, static UFUNCTIONs.
//
//   static float VerticalKick(int32 ShotIndex, float BasePitch, float ClimbPerShot, float MaxPitch);
//     // return FMath::Min(BasePitch + ShotIndex * ClimbPerShot, MaxPitch);   // ShotIndex 0-based
//   static float HorizontalKick(int32 ShotIndex, float BaseYaw);
//     // deterministic zig-zag: return (ShotIndex % 2 == 0) ? BaseYaw : -BaseYaw;
//   static float RecoverToward(float Current, float RecoverStep);
//     // move the accumulated offset toward 0 by at most RecoverStep:
//     // if (FMath::Abs(Current) <= RecoverStep) return 0.0f;
//     // return Current - (Current > 0.0f ? RecoverStep : -RecoverStep);
//
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Combat/EmergeRecoil.h"

BEGIN_DEFINE_SPEC(FEmergeRecoilSpec, "Emergence.Combat.Recoil",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeRecoilSpec)
void FEmergeRecoilSpec::Define()
{
	Describe("VerticalKick", [this]()
	{
		It("climbs per shot, capped at max", [this]()
		{
			TestEqual(TEXT("shot 0 -> base"), UEmergeRecoil::VerticalKick(0, 1.0f, 0.5f, 5.0f), 1.0f);
			TestEqual(TEXT("shot 1 -> 1.5"), UEmergeRecoil::VerticalKick(1, 1.0f, 0.5f, 5.0f), 1.5f);
			TestEqual(TEXT("shot 2 -> 2.0"), UEmergeRecoil::VerticalKick(2, 1.0f, 0.5f, 5.0f), 2.0f);
			TestEqual(TEXT("shot 10 caps at 5"), UEmergeRecoil::VerticalKick(10, 1.0f, 0.5f, 5.0f), 5.0f);
		});
	});

	Describe("HorizontalKick", [this]()
	{
		It("zig-zags right/left by shot parity", [this]()
		{
			TestEqual(TEXT("shot 0 right"), UEmergeRecoil::HorizontalKick(0, 0.3f), 0.3f);
			TestEqual(TEXT("shot 1 left"), UEmergeRecoil::HorizontalKick(1, 0.3f), -0.3f);
			TestEqual(TEXT("shot 2 right"), UEmergeRecoil::HorizontalKick(2, 0.3f), 0.3f);
		});
	});

	Describe("RecoverToward", [this]()
	{
		It("moves the muzzle offset toward center, not past it", [this]()
		{
			TestEqual(TEXT("2.0 by 0.5 -> 1.5"), UEmergeRecoil::RecoverToward(2.0f, 0.5f), 1.5f);
			TestEqual(TEXT("within step snaps to 0"), UEmergeRecoil::RecoverToward(0.3f, 0.5f), 0.0f);
			TestEqual(TEXT("negative recovers up"), UEmergeRecoil::RecoverToward(-2.0f, 0.5f), -1.5f);
			TestEqual(TEXT("already centered"), UEmergeRecoil::RecoverToward(0.0f, 0.5f), 0.0f);
		});
	});
}
#endif
