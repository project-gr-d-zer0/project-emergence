// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: survival needs COMPONENT (the playable loop bridge).
// THE PIVOT MODULE: a ticking UActorComponent (same shape as the existing UEmergeVitalsComponent) that puts the
// RICH survival modules INTO the pawn's loop. Each Simulate() advances Energy/Water/Blood via UEmergeSurvivorTick
// (which composes Metabolism+Bleeding+Disease). This is the bridge from tested-formula-library -> live gameplay on
// AEmergeCharacter (add it alongside the existing Vitals component). Tested by driving Simulate() over ticks with
// METAMORPHIC relations - no world needed (pure state math), so it stays headless-testable.
// CONTRACT — NEW class UEmergeSurvivalNeedsComponent : public UActorComponent
//   in Public/Survival/EmergeSurvivalNeedsComponent.h + Private/Survival/EmergeSurvivalNeedsComponent.cpp.
//   Mirror UEmergeVitalsComponent EXACTLY: UCLASS(ClassGroup=(Emerge), meta=(BlueprintSpawnableComponent)); the
//   .cpp #includes "Survival/EmergeSurvivorTick.h"; all logic funnels through Simulate() (TickComponent just calls it).
//
//   UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Emerge|Needs") float Energy = 5000.0f;
//   UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Emerge|Needs") float Water = 5000.0f;
//   UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Emerge|Needs") float Blood = 5000.0f;
//   UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Emerge|Needs") int32 Movement = 0;         // 0 Idle..3 Sprint
//   UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Emerge|Needs") int32 BleedingSources = 0;
//
//   UFUNCTION(BlueprintCallable, Category="Emerge|Needs") void Simulate(float DeltaSeconds);
//     // Energy = UEmergeSurvivorTick::DrainedEnergy(Energy, Movement, DeltaSeconds);
//     // Water  = UEmergeSurvivorTick::DrainedEnergy(Water,  Movement, DeltaSeconds);
//     // Blood  = UEmergeSurvivorTick::DrainedBlood(Blood, 2, BleedingSources, DeltaSeconds);
//   UFUNCTION(BlueprintPure, Category="Emerge|Needs") bool IsAlive() const;   // return Blood > 2500.0f;  // BLOOD_THRESHOLD_FATAL
//   UFUNCTION(BlueprintPure, Category="Emerge|Needs") int32 ImmunityLevel() const;
//     // return UEmergeSurvivorTick::ImmunityFromState(Energy, Water, 1.0f, Blood / 5000.0f);
//   virtual void TickComponent(float, ELevelTick, FActorComponentTickFunction*) override;   // just Simulate(DeltaTime)
//
// Deterministic; metamorphic relations by driving Simulate() over ticks.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Survival/EmergeSurvivalNeedsComponent.h"

BEGIN_DEFINE_SPEC(FEmergeSurvivalNeedsComponentSpec, "Emergence.Survival.NeedsComponent",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeSurvivalNeedsComponentSpec)
void FEmergeSurvivalNeedsComponentSpec::Define()
{
	Describe("Ticking survival loop on a component", [this]()
	{
		It("energy drains over ticks; sprint drains faster; bleeding drains blood; death on blood-out", [this]()
		{
			UEmergeSurvivalNeedsComponent* Idle = NewObject<UEmergeSurvivalNeedsComponent>();
			UEmergeSurvivalNeedsComponent* Sprint = NewObject<UEmergeSurvivalNeedsComponent>();
			Sprint->Movement = 3;
			for (int32 i = 0; i < 10; ++i) { Idle->Simulate(1.0f); Sprint->Simulate(1.0f); }
			TestTrue(TEXT("energy drained from full"), Idle->Energy < 5000.0f);
			TestTrue(TEXT("sprint drained more than idle"), Sprint->Energy < Idle->Energy);
			TestTrue(TEXT("energy stays >= 0"), Sprint->Energy >= 0.0f);

			UEmergeSurvivalNeedsComponent* Bleeder = NewObject<UEmergeSurvivalNeedsComponent>();
			Bleeder->BleedingSources = 3;
			const float bloodBefore = Bleeder->Blood;
			Bleeder->Simulate(1.0f);
			TestTrue(TEXT("bleeding drained blood"), Bleeder->Blood < bloodBefore);

			UEmergeSurvivalNeedsComponent* Healthy = NewObject<UEmergeSurvivalNeedsComponent>();
			TestTrue(TEXT("full blood -> alive"), Healthy->IsAlive());
			Healthy->Blood = 2000.0f;
			TestFalse(TEXT("bled out -> dead"), Healthy->IsAlive());

			UEmergeSurvivalNeedsComponent* Imm = NewObject<UEmergeSurvivalNeedsComponent>();
			TestEqual(TEXT("full vitals -> GREAT immunity (0)"), Imm->ImmunityLevel(), 0);
		});
	});
}
#endif
