#include "AutomationTestBase.h"
#include "EmergenceItemsDefinitions.h" // Assuming this is where UEmergeItemDefinition and related classes are defined

#if WITH_AUTOMATION_TESTS

BEGIN_DEFINE_SPEC(FDurabilityFeatureSpec, "Emergence.Items.Durability", EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
DEFINE_SPEC(FDurabilityFeatureSpec)

void FDurabilityFeatureSpec::Define()
{
    Describe("UEmergeItemDefinition Durability Feature", [this]()
    {
        It("Should have MaxDurability property and IsBroken method", [this]()
        {
            // Construct UEmergeItemDefinition
            auto ItemDef = NewObject<UEmergeItemDefinition>(GetTransientPackage());
            
            // Assert that MaxDurability is present and clamped to minimum 0
            TestTrueExpr(ItemDef->MaxDurability >= 0.0f);

            // Assuming UEmergeItemInstance or component has the IsBroken method.
            auto ItemInst = NewObject<UEmergeItemInstance>(GetTransientPackage());
            
            // Set current durability to a value greater than 0 and check if it's not broken
            ItemInst->CurrentDurability = 1.0f;
            TestFalseExpr(ItemInst->IsBroken());

            // Set current durability to 0 or less and check if it is broken
            ItemInst->CurrentDurability = 0.0f;
            TestTrueExpr(ItemInst->IsBroken());
            
            ItemInst->CurrentDurability = -1.0f;
            TestTrueExpr(ItemInst->IsBroken());
        });
    });
}

#endif // WITH_AUTOMATION_TESTS