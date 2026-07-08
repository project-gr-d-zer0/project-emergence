#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Inventory/EmergeInventoryComponent.h"
#include "Items/EmergeItemDefinition.h"
#include "UObject/Package.h"

BEGIN_DEFINE_SPEC(FEmergeInvGuardSpec3, "Emergence.Inventory.Guard3",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeInvGuardSpec3)

void FEmergeInvGuardSpec3::Define()
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
			TestFalse(TEXT("adding quantity <= 0 is rejected"), Inv->TryAddItem(Item, 0));
			TestEqual(TEXT("quantity remains 0"), Inv->GetQuantity(Item), 0);
		});

		It("removing more than held returns only what was held", [this]()
		{
			UEmergeInventoryComponent* Inv = NewObject<UEmergeInventoryComponent>(GetTransientPackage());
			Inv->MaxWeightKg = 10.0f;
			UEmergeItemDefinition* Item = NewObject<UEmergeItemDefinition>(GetTransientPackage());
			Item->WeightKg = 2.0f;
			Inv->TryAddItem(Item, 3);
			TestEqual(TEXT("removed all held"), Inv->RemoveItem(Item, 5), 3);
			TestEqual(TEXT("quantity is now 0"), Inv->GetQuantity(Item), 0);
		});

		It("adding the exact weight boundary item succeeds", [this]()
		{
			UEmergeInventoryComponent* Inv = NewObject<UEmergeInventoryComponent>(GetTransientPackage());
			Inv->MaxWeightKg = 10.0f;
			UEmergeItemDefinition* Item = NewObject<UEmergeItemDefinition>(GetTransientPackage());
			Item->WeightKg = 10.0f;
			TestTrue(TEXT("adding exact weight boundary item succeeds"), Inv->TryAddItem(Item, 1));
			TestEqual(TEXT("quantity is 1"), Inv->GetQuantity(Item), 1);
		});

		It("stacking the same item accumulates quantity", [this]()
		{
			UEmergeInventoryComponent* Inv = NewObject<UEmergeInventoryComponent>(GetTransientPackage());
			Inv->MaxWeightKg = 20.0f;
			UEmergeItemDefinition* Item = NewObject<UEmergeItemDefinition>(GetTransientPackage());
			Item->WeightKg = 5.0f;
			Inv->TryAddItem(Item, 1);
			TestTrue(TEXT("second add succeeds"), Inv->TryAddItem(Item, 2));
			TestEqual(TEXT("quantity is 3"), Inv->GetQuantity(Item), 3);
		});

		It("returns 0 for GetQuantity of an unheld item", [this]()
		{
			UEmergeInventoryComponent* Inv = NewObject<UEmergeInventoryComponent>(GetTransientPackage());
			UEmergeItemDefinition* Item1 = NewObject<UEmergeItemDefinition>(GetTransientPackage());
			Item1->WeightKg = 2.0f;
			Inv->TryAddItem(Item1, 3);
			UEmergeItemDefinition* Item2 = NewObject<UEmergeItemDefinition>(GetTransientPackage());
			TestEqual(TEXT("quantity of unheld item is 0"), Inv->GetQuantity(Item2), 0);
		});
	});
}
#endif