// HUMAN-AUTHORED acceptance test (trusted). The builder loop implements against this; it must NOT edit it.
// Feature: TryAddItem must enforce per-item MaxStack — an add that would push a single item's quantity
// above its MaxStack is rejected and adds nothing. Weight is deliberately non-binding here.
#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Inventory/EmergeInventoryComponent.h"
#include "Items/EmergeItemDefinition.h"
#include "UObject/Package.h"

BEGIN_DEFINE_SPEC(FEmergeMaxStackSpec, "Emergence.Inventory.MaxStack",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeMaxStackSpec)
void FEmergeMaxStackSpec::Define()
{
	Describe("MaxStack enforcement", [this]()
	{
		It("allows filling up to MaxStack but rejects going over", [this]()
		{
			UEmergeInventoryComponent* Inv = NewObject<UEmergeInventoryComponent>(GetTransientPackage());
			Inv->MaxWeightKg = 1000.0f; // weight is not the binding constraint in this test
			UEmergeItemDefinition* Item = NewObject<UEmergeItemDefinition>(GetTransientPackage());
			Item->WeightKg = 1.0f;
			Item->MaxStack = 5;
			TestTrue(TEXT("adding exactly MaxStack (5) succeeds"), Inv->TryAddItem(Item, 5));
			TestFalse(TEXT("adding 1 more would exceed MaxStack -> rejected"), Inv->TryAddItem(Item, 1));
			TestEqual(TEXT("quantity stays capped at MaxStack"), Inv->GetQuantity(Item), 5);
		});

		It("rejects a single add larger than MaxStack", [this]()
		{
			UEmergeInventoryComponent* Inv = NewObject<UEmergeInventoryComponent>(GetTransientPackage());
			Inv->MaxWeightKg = 1000.0f;
			UEmergeItemDefinition* Item = NewObject<UEmergeItemDefinition>(GetTransientPackage());
			Item->WeightKg = 1.0f;
			Item->MaxStack = 3;
			TestFalse(TEXT("adding 4 at once exceeds MaxStack(3) -> rejected"), Inv->TryAddItem(Item, 4));
			TestEqual(TEXT("nothing was added"), Inv->GetQuantity(Item), 0);
		});
	});
}
#endif
