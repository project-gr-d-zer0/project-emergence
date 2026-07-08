#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Inventory/EmergeInventoryComponent.h"
#include "Items/EmergeItemDefinition.h"
#include "UObject/Package.h"

BEGIN_DEFINE_SPEC(FEmergeInvGuardSpec1, "Emergence.Inventory.Guard1",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeInvGuardSpec1)

void FEmergeInvGuardSpec1::Define()
{
	Describe("UEmergeInventoryComponent", [this]()
	{
		It("rejects adding a null item", [this]()
		{
			UEmergeInventoryComponent* Inv = NewObject<UEmergeInventoryComponent>(GetTransientPackage());
			TestFalse(TEXT("adding null item is rejected"), Inv->TryAddItem(nullptr, 1));
		});

		It("rejects adding non-positive quantity", [this]()
		{
			UEmergeInventoryComponent* Inv = NewObject<UEmergeInventoryComponent>(GetTransientPackage());
			Inv->MaxWeightKg = 10.0f;
			UEmergeItemDefinition* Item = NewObject<UEmergeItemDefinition>(GetTransientPackage());
			Item->WeightKg = 2.0f;

			TestFalse(TEXT("adding zero quantity is rejected"), Inv->TryAddItem(Item, 0));
			TestFalse(TEXT("adding negative quantity is rejected"), Inv->TryAddItem(Item, -1));
		});

		It("removing more than held returns only what was held", [this]()
		{
			UEmergeInventoryComponent* Inv = NewObject<UEmergeInventoryComponent>(GetTransientPackage());
			Inv->MaxWeightKg = 10.0f;
			UEmergeItemDefinition* Item = NewObject<UEmergeItemDefinition>(GetTransientPackage());
			Item->WeightKg = 2.0f;

			Inv->TryAddItem(Item, 3);
			TestEqual(TEXT("removed only what was held"), Inv->RemoveItem(Item, 5), 3);
			TestEqual(TEXT("quantity is now zero"), Inv->GetQuantity(Item), 0);
		});

		It("adding items at exact weight boundary", [this]()
		{
			UEmergeInventoryComponent* Inv = NewObject<UEmergeInventoryComponent>(GetTransientPackage());
			Inv->MaxWeightKg = 10.0f;
			UEmergeItemDefinition* Item = NewObject<UEmergeItemDefinition>(GetTransientPackage());
			Item->WeightKg = 2.0f;

			TestTrue(TEXT("adding items up to max weight succeeds"), Inv->TryAddItem(Item, 5));
			TestFalse(TEXT("adding one more item exceeds max weight"), Inv->TryAddItem(Item, 1));
		});

		It("stacking the same item accumulates quantity", [this]()
		{
			UEmergeInventoryComponent* Inv = NewObject<UEmergeInventoryComponent>(GetTransientPackage());
			Inv->MaxWeightKg = 20.0f;
			UEmergeItemDefinition* Item = NewObject<UEmergeItemDefinition>(GetTransientPackage());
			Item->WeightKg = 2.0f;

			TestTrue(TEXT("first add succeeds"), Inv->TryAddItem(Item, 3));
			TestTrue(TEXT("second add accumulates quantity"), Inv->TryAddItem(Item, 4));
			TestEqual(TEXT("total quantity is 7"), Inv->GetQuantity(Item), 7);
		});

		It("returns zero for unheld item", [this]()
		{
			UEmergeInventoryComponent* Inv = NewObject<UEmergeInventoryComponent>(GetTransientPackage());
			UEmergeItemDefinition* Item1 = NewObject<UEmergeItemDefinition>(GetTransientPackage());
			Item1->WeightKg = 2.0f;
			UEmergeItemDefinition* Item2 = NewObject<UEmergeItemDefinition>(GetTransientPackage());
			Item2->WeightKg = 3.0f;

			TestTrue(TEXT("adding one item succeeds"), Inv->TryAddItem(Item1, 1));
			TestEqual(TEXT("quantity of unheld item is zero"), Inv->GetQuantity(Item2), 0);
		});
	});
}
#endif