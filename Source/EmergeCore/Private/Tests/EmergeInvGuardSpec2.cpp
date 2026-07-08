#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Inventory/EmergeInventoryComponent.h"
#include "Items/EmergeItemDefinition.h"
#include "UObject/Package.h"

BEGIN_DEFINE_SPEC(FEmergeInvGuardSpec2, "Emergence.Inventory.Guard2",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeInvGuardSpec2)

void FEmergeInvGuardSpec2::Define()
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
			Item->WeightKg = 1.0f;
			TestFalse(TEXT("adding Quantity <= 0 is rejected"), Inv->TryAddItem(Item, 0));
			TestFalse(TEXT("adding negative quantity is rejected"), Inv->TryAddItem(Item, -1));
		});

		It("removing more than held returns only what was held", [this]()
		{
			UEmergeInventoryComponent* Inv = NewObject<UEmergeInventoryComponent>(GetTransientPackage());
			UEmergeItemDefinition* Item = NewObject<UEmergeItemDefinition>(GetTransientPackage());
			Item->WeightKg = 1.0f;
			Inv->TryAddItem(Item, 2);
			TestEqual(TEXT("removed 2"), Inv->RemoveItem(Item, 3), 2);
			TestEqual(TEXT("quantity is now 0"), Inv->GetQuantity(Item), 0);
		});

		It("adding items of the same type accumulates quantity", [this]()
		{
			UEmergeInventoryComponent* Inv = NewObject<UEmergeInventoryComponent>(GetTransientPackage());
			UEmergeItemDefinition* Item = NewObject<UEmergeItemDefinition>(GetTransientPackage());
			Item->WeightKg = 1.0f;
			TestTrue(TEXT("first add succeeds"), Inv->TryAddItem(Item, 2));
			TestTrue(TEXT("second add succeeds"), Inv->TryAddItem(Item, 3));
			TestEqual(TEXT("quantity is now 5"), Inv->GetQuantity(Item), 5);
		});

		It("GetQuantity of an unheld item returns 0", [this]()
		{
			UEmergeInventoryComponent* Inv = NewObject<UEmergeInventoryComponent>(GetTransientPackage());
			UEmergeItemDefinition* Item1 = NewObject<UEmergeItemDefinition>(GetTransientPackage());
			Item1->WeightKg = 1.0f;
			Inv->TryAddItem(Item1, 2);
			UEmergeItemDefinition* Item2 = NewObject<UEmergeItemDefinition>(GetTransientPackage());
			TestEqual(TEXT("unheld item quantity is 0"), Inv->GetQuantity(Item2), 0);
		});
	});
}
#endif