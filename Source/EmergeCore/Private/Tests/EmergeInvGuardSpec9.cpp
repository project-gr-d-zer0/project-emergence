#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Inventory/EmergeInventoryComponent.h"
#include "Items/EmergeItemDefinition.h"
#include "UObject/Package.h"

BEGIN_DEFINE_SPEC(FEmergeInvGuardSpec9, "Emergence.Inventory.Guard9",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeInvGuardSpec9)

void FEmergeInvGuardSpec9::Define()
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

		It("adding items at exact weight boundary", [this]()
		{
			UEmergeInventoryComponent* Inv = NewObject<UEmergeInventoryComponent>(GetTransientPackage());
			Inv->MaxWeightKg = 10.0f;
			UEmergeItemDefinition* Item = NewObject<UEmergeItemDefinition>(GetTransientPackage());
			Item->WeightKg = 2.5f;

			TestTrue(TEXT("add succeeds at exact boundary"), Inv->TryAddItem(Item, 4));
			TestFalse(TEXT("adding one more exceeds weight limit"), Inv->TryAddItem(Item, 1));

			TestEqual(TEXT("quantity is 4"), Inv->GetQuantity(Item), 4);
			TestEqual(TEXT("weight is exactly 10kg"), Inv->GetCurrentWeightKg(), 10.0f);
		});

		It("stacking the same item accumulates quantity", [this]()
		{
			UEmergeInventoryComponent* Inv = NewObject<UEmergeInventoryComponent>(GetTransientPackage());
			Inv->MaxWeightKg = 20.0f;
			UEmergeItemDefinition* Item = NewObject<UEmergeItemDefinition>(GetTransientPackage());
			Item->WeightKg = 1.5f;

			TestTrue(TEXT("first add succeeds"), Inv->TryAddItem(Item, 3));
			TestTrue(TEXT("second add succeeds and accumulates"), Inv->TryAddItem(Item, 2));

			TestEqual(TEXT("quantity is 5"), Inv->GetQuantity(Item), 5);
		});

		It("returns zero for unheld item quantity", [this]()
		{
			UEmergeInventoryComponent* Inv = NewObject<UEmergeInventoryComponent>(GetTransientPackage());
			Inv->MaxWeightKg = 10.0f;
			UEmergeItemDefinition* Item = NewObject<UEmergeItemDefinition>(GetTransientPackage());
			Item->WeightKg = 2.5f;

			TestEqual(TEXT("quantity of unheld item is zero"), Inv->GetQuantity(Item), 0);
		});
	});
}
#endif