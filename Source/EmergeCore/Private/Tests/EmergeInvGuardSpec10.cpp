#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Inventory/EmergeInventoryComponent.h"
#include "Items/EmergeItemDefinition.h"
#include "UObject/Package.h"

BEGIN_DEFINE_SPEC(FEmergeInvGuardSpec10, "Emergence.Inventory.Guard10",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeInvGuardSpec10)

void FEmergeInvGuardSpec10::Define()
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
			TestEqual(TEXT("removed all held"), Inv->RemoveItem(Item, 5), 3);
			TestEqual(TEXT("quantity is now zero"), Inv->GetQuantity(Item), 0);
		});

		It("adding items of the same type accumulates quantity", [this]()
		{
			UEmergeInventoryComponent* Inv = NewObject<UEmergeInventoryComponent>(GetTransientPackage());
			Inv->MaxWeightKg = 10.0f;
			UEmergeItemDefinition* Item = NewObject<UEmergeItemDefinition>(GetTransientPackage());
			Item->WeightKg = 2.0f;
			TestTrue(TEXT("first add succeeds"), Inv->TryAddItem(Item, 3));
			TestTrue(TEXT("second add succeeds"), Inv->TryAddItem(Item, 2));
			TestEqual(TEXT("total quantity is 5"), Inv->GetQuantity(Item), 5);
		});

		It("get quantity of an unheld item returns zero", [this]()
		{
			UEmergeInventoryComponent* Inv = NewObject<UEmergeInventoryComponent>(GetTransientPackage());
			Inv->MaxWeightKg = 10.0f;
			UEmergeItemDefinition* Item = NewObject<UEmergeItemDefinition>(GetTransientPackage());
			Item->WeightKg = 2.0f;
			TestEqual(TEXT("unheld item quantity is zero"), Inv->GetQuantity(Item), 0);
		});
	});
}
#endif