#include "AutomationTestBase.h"
#include "InventoryComponent.h" // Assuming InventoryComponent is your inventory class

BEGIN_DEFINE_SPEC(FRemoveClampSpec, "Emergence.Inventory.RemoveClamp", EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
#if WITH_AUTOMATION_TESTS

void Define()
{
    UsingNamespace(UE5);

    Describe("Inventory Remove Clamp Functionality", [this]()
    {
        It("Should not remove more items than available and return correct quantity", [this]()
        {
            // Create a new InventoryComponent
            AActor* TestActor = NewObject<AActor>(GetTransientPackage());
            UInventoryComponent* InventoryComp = NewObject<UInventoryComponent>(TestActor);

            // Add some items to the inventory
            const FString ItemName = TEXT("TestItem");
            int32 InitialQuantity = 5;
            InventoryComp->AddItem(ItemName, InitialQuantity);

            // Attempt to remove more than available
            int32 QuantityToRemove = 10; // More than initial quantity
            int32 RemovedQuantity = InventoryComp->RemoveItem(ItemName, QuantityToRemove);

            // Assert that the removed quantity is equal to the initial quantity and not the requested amount
            EXPECT_EQ(RemovedQuantity, InitialQuantity) << "Removed more items than available";

            // Check if the item's quantity in inventory is now 0
            int32 RemainingQuantity = InventoryComp->GetItemQuantity(ItemName);
            EXPECT_EQ(RemainingQuantity, 0) << "Item quantity should be zero after removal";

            // Attempt to remove from an empty/absent item and check return value
            RemovedQuantity = InventoryComp->RemoveItem(TEXT("NonExistentItem"), QuantityToRemove);
            EXPECT_EQ(RemovedQuantity, 0) << "Removing from non-existent or empty item should return 0";
        });
    });
}

#endif // WITH_AUTOMATION_TESTS
END_DEFINE_SPEC(FRemoveClampSpec)