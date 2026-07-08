#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Inventory/EmergeInventoryComponent.h"
#include "Items/EmergeItemDefinition.h"
#include "UObject/Package.h"

BEGIN_DEFINE_SPEC(FEmergeInvGuardSpec5, "Emergence.Inventory.Guard5",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeInvGuardSpec5)

void FEmergeInvGuardSpec5::Define()
{
	Describe("UEmergeInventoryComponent", [this]()
	{
		It("rejects adding a null item", [this]()
		{
			UEmergeInventoryComponent* Inv = NewObject<UEmergeInventoryComponent>(GetTransientPackage());
			TestFalse(TEXT("adding null item is rejected"), Inv->TryAddItem(nullptr, 1));
		});

		It("rejects adding Quantity <= 0 items", [this]()
		{
			UEmergeInventoryComponent* Inv = NewObject<UEmergeInventoryComponent>(GetTransientPackage());
			UEmergeItemDefinition* Item = NewObject<UEmergeItemDefinition>(GetTransientPackage());
			Item->WeightKg = 2.0f;
			TestFalse(TEXT("adding quantity <= 0 is rejected"), Inv->TryAddItem(Item, 0));
			TestFalse(TEXT("adding negative quantity is rejected"), Inv->TryAddItem(Item, -1));
		});

		It("removing more than held returns only what was held", [this]()
		{
			UEmergeInventoryComponent* Inv = NewObject<UEmergeInventoryComponent>(GetTransientPackage());
			Inv->MaxWeightKg = 10.0f;
			UEmergeItemDefinition* Item = NewObject<UEmergeItemDefinition>(GetTransientPackage());
			Item->WeightKg = 2.0f;
			Inv->TryAddItem(Item, 3);
			TestEqual(TEXT("removed 3"), Inv->RemoveItem(Item, 5), 3);
			TestEqual(TEXT("quantity is now 0"), Inv->GetQuantity(Item), 0);
		});

		It("adding items at the exact weight boundary is allowed", [this]()
		{
			UEmergeInventoryComponent* Inv = NewObject<UEmergeInventoryComponent>(GetTransientPackage());
			Inv->MaxWeightKg = 10.0f;
			UEmergeItemDefinition* Item = NewObject<UEmergeItemDefinition>(GetTransientPackage());
			Item->WeightKg = 10.0f;
			TestTrue(TEXT("adding item at exact weight boundary is allowed"), Inv->TryAddItem(Item, 1));
			TestFalse(TEXT("adding one more would exceed max weight"), Inv->TryAddItem(Item, 1));
		});

		It("stacking the same item accumulates quantity", [this]()
		{
			UEmergeInventoryComponent* Inv = NewObject<UEmergeInventoryComponent>(GetTransientPackage());
			Inv->MaxWeightKg = 20.0f;
			UEmergeItemDefinition* Item = NewObject<UEmergeItemDefinition>(GetTransientPackage());
			Item->WeightKg = 5.0f;
			TestTrue(TEXT("first add succeeds"), Inv->TryAddItem(Item, 1));
			TestTrue(TEXT("second add succeeds"), Inv->TryAddItem(Item, 2));
			TestEqual(TEXT("quantity is now 3"), Inv->GetQuantity(Item), 3);
		});

		It("returns 0 for GetQuantity of an unheld item", [this]()
		{
			UEmergeInventoryComponent* Inv = NewObject<UEmergeInventoryComponent>(GetTransientPackage());
			UEmergeItemDefinition* Item1 = NewObject<UEmergeItemDefinition>(GetTransientPackage());
			Item1->WeightKg = 2.0f;
			UEmergeItemDefinition* Item2 = NewObject<UEmergeItemDefinition>(GetTransientPackage());
			Item2->WeightKg = 3.0f;
			TestTrue(TEXT("adding item1 succeeds"), Inv->TryAddItem(Item1, 1));
			TestEqual(TEXT("quantity of unheld item is 0"), Inv->GetQuantity(Item2), 0);
		});
	});
}
#endif