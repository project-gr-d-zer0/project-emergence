#include "Misc/AutomationTest.h"
#if WITH_AUTOMATION_TESTS
#include "Inventory/EmergeInventoryComponent.h"
#include "Items/EmergeItemDefinition.h"
#include "UObject/Package.h"

BEGIN_DEFINE_SPEC(FEmergeInventorySpec, "Emergence.Inventory.Core",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
END_DEFINE_SPEC(FEmergeInventorySpec)
void FEmergeInventorySpec::Define()
{
	Describe("UEmergeInventoryComponent", [this]()
	{
		It("adds items and tracks total weight", [this]()
		{
			UEmergeInventoryComponent* Inv = NewObject<UEmergeInventoryComponent>(GetTransientPackage());
			Inv->MaxWeightKg = 10.0f;
			UEmergeItemDefinition* Item = NewObject<UEmergeItemDefinition>(GetTransientPackage());
			Item->WeightKg = 2.0f;
			TestTrue(TEXT("first add succeeds"), Inv->TryAddItem(Item, 3));
			TestEqual(TEXT("quantity is 3"), Inv->GetQuantity(Item), 3);
			TestEqual(TEXT("weight is 6kg"), Inv->GetCurrentWeightKg(), 6.0f);
		});

		It("rejects an add that would exceed max weight", [this]()
		{
			UEmergeInventoryComponent* Inv = NewObject<UEmergeInventoryComponent>(GetTransientPackage());
			Inv->MaxWeightKg = 5.0f;
			UEmergeItemDefinition* Item = NewObject<UEmergeItemDefinition>(GetTransientPackage());
			Item->WeightKg = 2.0f;
			TestTrue(TEXT("add 2 ok (4kg)"), Inv->TryAddItem(Item, 2));
			TestFalse(TEXT("add 1 more rejected (would be 6kg > 5)"), Inv->TryAddItem(Item, 1));
			TestEqual(TEXT("quantity still 2"), Inv->GetQuantity(Item), 2);
		});

		It("removes items and updates weight", [this]()
		{
			UEmergeInventoryComponent* Inv = NewObject<UEmergeInventoryComponent>(GetTransientPackage());
			Inv->MaxWeightKg = 100.0f;
			UEmergeItemDefinition* Item = NewObject<UEmergeItemDefinition>(GetTransientPackage());
			Item->WeightKg = 1.5f;
			Inv->TryAddItem(Item, 4);
			TestEqual(TEXT("removed 3"), Inv->RemoveItem(Item, 3), 3);
			TestEqual(TEXT("1 left"), Inv->GetQuantity(Item), 1);
			TestEqual(TEXT("weight is 1.5kg"), Inv->GetCurrentWeightKg(), 1.5f);
		});
	});
}
#endif
