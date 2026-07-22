// HUMAN-AUTHORED trusted acceptance spec (do not edit). Feature: encumbrance load tiers.
// CONTRACT to implement — add to UEmergeInventoryComponent:
//   int32 GetLoadTier() const;
//   ratio = (MaxWeightKg > 0) ? GetCurrentWeightKg() / MaxWeightKg : 0
//     tier 0 (Light)  when ratio <= 0.50
//     tier 1 (Normal) when 0.50 < ratio <= 0.75
//     tier 2 (Heavy)  when ratio > 0.75
// Deterministic, single behavior, exact golden values, boundaries covered. Weight is filled with 1kg items.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Inventory/EmergeInventoryComponent.h"
#include "Items/EmergeItemDefinition.h"
#include "UObject/Package.h"

BEGIN_DEFINE_SPEC(FEmergeEncumbranceSpec, "Emergence.Inventory.Encumbrance",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
static UEmergeInventoryComponent* InvWithKg(float MaxKg, int32 FillKg);
END_DEFINE_SPEC(FEmergeEncumbranceSpec)

UEmergeInventoryComponent* FEmergeEncumbranceSpec::InvWithKg(float MaxKg, int32 FillKg)
{
	UEmergeInventoryComponent* Inv = NewObject<UEmergeInventoryComponent>(GetTransientPackage());
	Inv->MaxWeightKg = MaxKg;
	UEmergeItemDefinition* Item = NewObject<UEmergeItemDefinition>(GetTransientPackage());
	Item->WeightKg = 1.0f;
	Item->MaxStack = 1000000;
	Inv->TryAddItem(Item, FillKg);
	return Inv;
}

void FEmergeEncumbranceSpec::Define()
{
	Describe("UEmergeInventoryComponent::GetLoadTier (MaxWeight=100)", [this]()
	{
		It("returns the exact tier at each band and boundary", [this]()
		{
			TestEqual(TEXT("40kg (0.40) -> Light 0"),       InvWithKg(100.f, 40)->GetLoadTier(), 0);
			TestEqual(TEXT("50kg (0.50) boundary -> 0"),     InvWithKg(100.f, 50)->GetLoadTier(), 0);
			TestEqual(TEXT("60kg (0.60) -> Normal 1"),       InvWithKg(100.f, 60)->GetLoadTier(), 1);
			TestEqual(TEXT("75kg (0.75) boundary -> 1"),     InvWithKg(100.f, 75)->GetLoadTier(), 1);
			TestEqual(TEXT("90kg (0.90) -> Heavy 2"),        InvWithKg(100.f, 90)->GetLoadTier(), 2);
			TestEqual(TEXT("100kg (1.00) -> Heavy 2"),       InvWithKg(100.f, 100)->GetLoadTier(), 2);
			TestEqual(TEXT("empty (0.00) -> Light 0"),       InvWithKg(100.f, 0)->GetLoadTier(), 0);
		});
	});
}
#endif
