// EmergenceItemsDurabilitySpec.cpp
//
// Automation Spec for item durability feature.
// This spec is intentionally written to FAIL until the durability feature
// is implemented:
//   - UEmergeItemDefinition::MaxDurability (float, ClampMin 0)
//   - UEmergeItemInstance::IsBroken(float CurrentDurability) -> bool
//
// Deterministic, unit-testable, no rendering.

#if WITH_AUTOMATION_TESTS

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "UObject/Package.h"
#include "UObject/UnrealType.h"
#include "UObject/UnrealTypePrivate.h"

// Feature headers. If these are not present yet, the test target will fail
// to compile which is an acceptable "failing until implemented" state.
// Once headers exist, the runtime asserts below drive the failures.
#include "EmergeItemDefinition.h"
#include "EmergeItemInstance.h"

BEGIN_DEFINE_SPEC(FEmergenceItemsDurabilitySpec,
	"Emergence.Items.Durability",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)

	UEmergeItemDefinition* ItemDef = nullptr;
	UEmergeItemInstance* ItemInstance = nullptr;

END_DEFINE_SPEC(FEmergenceItemsDurabilitySpec)

void FEmergenceItemsDurabilitySpec::Define()
{
	BeforeEach([this]()
	{
		ItemDef = NewObject<UEmergeItemDefinition>(GetTransientPackage());
		ItemInstance = NewObject<UEmergeItemInstance>(GetTransientPackage());

		TestNotNull(TEXT("UEmergeItemDefinition should construct"), ItemDef);
		TestNotNull(TEXT("UEmergeItemInstance should construct"), ItemInstance);
	});

	Describe("UEmergeItemDefinition MaxDurability property", [this]()
	{
		It("should expose a float MaxDurability UPROPERTY", [this]()
		{
			if (!ItemDef)
			{
				AddError(TEXT("ItemDef was null; cannot inspect MaxDurability."));
				return;
			}

			const UClass* DefClass = ItemDef->GetClass();
			FProperty* Prop = DefClass->FindPropertyByName(FName(TEXT("MaxDurability")));

			TestNotNull(TEXT("MaxDurability property must exist on UEmergeItemDefinition"), Prop);

			if (Prop)
			{
				const FFloatProperty* FloatProp = CastField<FFloatProperty>(Prop);
				TestNotNull(TEXT("MaxDurability must be a float property"), FloatProp);
			}
		});

		It("should declare ClampMin=0 metadata on MaxDurability", [this]()
		{
			if (!ItemDef)
			{
				AddError(TEXT("ItemDef was null; cannot inspect MaxDurability metadata."));
				return;
			}

			const UClass* DefClass = ItemDef->GetClass();
			FProperty* Prop = DefClass->FindPropertyByName(FName(TEXT("MaxDurability")));

			if (!Prop)
			{
				AddError(TEXT("MaxDurability property missing; ClampMin metadata cannot be verified."));
				return;
			}

#if WITH_EDITORONLY_DATA
			const bool bHasClampMin = Prop->HasMetaData(TEXT("ClampMin"));
			TestTrue(TEXT("MaxDurability must specify ClampMin metadata"), bHasClampMin);

			if (bHasClampMin)
			{
				const FString ClampMinValue = Prop->GetMetaData(TEXT("ClampMin"));
				const float ClampMin = FCString::Atof(*ClampMinValue);
				TestEqual(TEXT("MaxDurability ClampMin must be 0"), ClampMin, 0.0f);
			}
#else
			AddWarning(TEXT("ClampMin metadata not available outside editor-only data; skipping metadata check."));
#endif
		});
	});

	Describe("UEmergeItemInstance IsBroken(currentDurability)", [this]()
	{
		It("should return true when currentDurability is exactly 0", [this]()
		{
			if (!ItemInstance)
			{
				AddError(TEXT("ItemInstance was null; cannot evaluate IsBroken."));
				return;
			}

			TestTrue(TEXT("IsBroken must be true when currentDurability == 0"),
				ItemInstance->IsBroken(0.0f));
		});

		It("should return true when currentDurability is negative", [this]()
		{
			if (!ItemInstance)
			{
				AddError(TEXT("ItemInstance was null; cannot evaluate IsBroken."));
				return;
			}

			TestTrue(TEXT("IsBroken must be true when currentDurability < 0"),
				ItemInstance->IsBroken(-5.0f));
		});

		It("should return false when currentDurability is positive", [this]()
		{
			if (!ItemInstance)
			{
				AddError(TEXT("ItemInstance was null; cannot evaluate IsBroken."));
				return;
			}

			TestFalse(TEXT("IsBroken must be false when currentDurability > 0"),
				ItemInstance->IsBroken(1.0f));
		});

		It("should return false for a small positive epsilon durability", [this]()
		{
			if (!ItemInstance)
			{
				AddError(TEXT("ItemInstance was null; cannot evaluate IsBroken."));
				return;
			}

			TestFalse(TEXT("IsBroken must be false when currentDurability is a small positive value"),
				ItemInstance->IsBroken(KINDA_SMALL_NUMBER));
		});

		It("should be deterministic across repeated calls", [this]()
		{
			if (!ItemInstance)
			{
				AddError(TEXT("ItemInstance was null; cannot evaluate IsBroken."));
				return;
			}

			const bool bFirst = ItemInstance->IsBroken(0.0f);
			const bool bSecond = ItemInstance->IsBroken(0.0f);
			const bool bThird = ItemInstance->IsBroken(0.0f);

			TestEqual(TEXT("IsBroken(0) must be deterministic (call 1 vs 2)"), bFirst, bSecond);
			TestEqual(TEXT("IsBroken(0) must be deterministic (call 2 vs 3)"), bSecond, bThird);
			TestTrue(TEXT("IsBroken(0) must consistently return true"), bFirst);
		});
	});

	AfterEach([this]()
	{
		ItemDef = nullptr;
		ItemInstance = nullptr;
	});
}

#endif // WITH_AUTOMATION_TESTS