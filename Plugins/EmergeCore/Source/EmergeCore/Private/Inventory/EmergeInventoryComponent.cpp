#include "Inventory/EmergeInventoryComponent.h"
#include "Items/EmergeItemDefinition.h"

UEmergeInventoryComponent::UEmergeInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

float UEmergeInventoryComponent::GetCurrentWeightKg() const
{
	float Total = 0.0f;
	for (const FEmergeInventoryEntry& E : Entries)
	{
		if (E.Item)
		{
			Total += E.Item->WeightKg * E.Quantity;
		}
	}
	return Total;
}

int32 UEmergeInventoryComponent::GetQuantity(const UEmergeItemDefinition* Item) const
{
	for (const FEmergeInventoryEntry& E : Entries)
	{
		if (E.Item == Item)
		{
			return E.Quantity;
		}
	}
	return 0;
}

bool UEmergeInventoryComponent::TryAddItem(UEmergeItemDefinition* Item, int32 Quantity)
{
	if (!Item || Quantity <= 0)
	{
		return false;
	}
	// Per-item MaxStack cap. Only enforced when the item opts into stacking (MaxStack > 1);
	// the default of 1 is treated as unbounded so existing weight-gated behavior is preserved.
	if (Item->MaxStack > 1 && GetQuantity(Item) + Quantity > Item->MaxStack)
	{
		return false;
	}
	const float Added = Item->WeightKg * Quantity;
	if (GetCurrentWeightKg() + Added > MaxWeightKg)
	{
		return false;
	}
	for (FEmergeInventoryEntry& E : Entries)
	{
		if (E.Item == Item)
		{
			E.Quantity += Quantity;
			return true;
		}
	}
	FEmergeInventoryEntry NewEntry;
	NewEntry.Item = Item;
	NewEntry.Quantity = Quantity;
	Entries.Add(NewEntry);
	return true;
}

int32 UEmergeInventoryComponent::RemoveItem(UEmergeItemDefinition* Item, int32 Quantity)
{
	if (!Item || Quantity <= 0)
	{
		return 0;
	}
	for (int32 i = 0; i < Entries.Num(); ++i)
	{
		if (Entries[i].Item == Item)
		{
			const int32 Removed = FMath::Min(Quantity, Entries[i].Quantity);
			Entries[i].Quantity -= Removed;
			if (Entries[i].Quantity <= 0)
			{
				Entries.RemoveAt(i);
			}
			return Removed;
		}
	}
	return 0;
}

float UEmergeInventoryComponent::GetLoadRatio() const
{
	if (MaxWeightKg <= 0.0f)
	{
		return 0.0f;
	}
	return GetCurrentWeightKg() / MaxWeightKg;
}

int32 UEmergeInventoryComponent::GetLoadTier() const
{
	const float Ratio = GetLoadRatio();
	if (Ratio > HeavyMinFraction)
	{
		return 2;
	}
	if (Ratio > LightMaxFraction)
	{
		return 1;
	}
	return 0;
}
