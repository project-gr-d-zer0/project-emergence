// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: interaction prompt priority (shell).
// SHELL stratum: the "press E to <verb>" selector - the core contextual-interaction logic of a 3D-Zomboid-style
// game. Decides whether a target is in reach, which of two candidates wins the prompt (higher priority, then
// nearer), and how strongly the prompt fades in as you approach. Pure selection math; deterministic.
// CONTRACT — NEW class UEmergeInteractionPrompt : public UBlueprintFunctionLibrary
//   in Public/Shell/EmergeInteractionPrompt.h + Private/Shell/EmergeInteractionPrompt.cpp, static UFUNCTIONs.
//
//   static bool  InRange(float Distance, float MaxRange);   // return Distance <= MaxRange;
//   static bool  IsHigherPriority(float DistA, int32 PrioA, float DistB, int32 PrioB);
//     // if (PrioA != PrioB) return PrioA > PrioB;   // higher priority wins
//     // return DistA < DistB;                       // tie -> nearer wins
//   static float PromptAlpha(float Distance, float MaxRange);
//     // fade in on approach: return FMath::Clamp(1.0f - Distance / MaxRange, 0.0f, 1.0f);
//
// Deterministic, exact golden values, boundaries covered.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Shell/EmergeInteractionPrompt.h"

BEGIN_DEFINE_SPEC(FEmergeInteractionPromptSpec, "Emergence.Shell.InteractionPrompt",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeInteractionPromptSpec)
void FEmergeInteractionPromptSpec::Define()
{
	Describe("InRange & PromptAlpha", [this]()
	{
		It("reach test (inclusive) and approach fade", [this]()
		{
			TestTrue(TEXT("2 within 3"), UEmergeInteractionPrompt::InRange(2.f, 3.f));
			TestTrue(TEXT("exactly at range"), UEmergeInteractionPrompt::InRange(3.f, 3.f));
			TestFalse(TEXT("4 beyond 3"), UEmergeInteractionPrompt::InRange(4.f, 3.f));
			TestEqual(TEXT("touching -> full alpha"), UEmergeInteractionPrompt::PromptAlpha(0.f, 3.f), 1.0f);
			TestEqual(TEXT("at range -> 0"), UEmergeInteractionPrompt::PromptAlpha(3.f, 3.f), 0.0f);
			TestEqual(TEXT("halfway -> 0.5"), UEmergeInteractionPrompt::PromptAlpha(1.5f, 3.f), 0.5f);
		});
	});

	Describe("IsHigherPriority", [this]()
	{
		It("higher priority wins; ties break on nearer", [this]()
		{
			TestTrue(TEXT("prio 2 beats prio 1 despite farther"), UEmergeInteractionPrompt::IsHigherPriority(5.f, 2, 1.f, 1));
			TestFalse(TEXT("same prio, A farther -> not higher"), UEmergeInteractionPrompt::IsHigherPriority(5.f, 1, 1.f, 1));
			TestTrue(TEXT("same prio, A nearer -> higher"), UEmergeInteractionPrompt::IsHigherPriority(1.f, 1, 5.f, 1));
			TestFalse(TEXT("lower prio loses"), UEmergeInteractionPrompt::IsHigherPriority(1.f, 0, 5.f, 2));
		});
	});
}
#endif
