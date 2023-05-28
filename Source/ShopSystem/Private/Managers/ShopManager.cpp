// Copyright shenkns Shop System Developed With Unreal Engine. All Rights Reserved 2023.

#include "Managers/ShopManager.h"

#include "Data/ShopCategoryData.h"
#include "Module/ShopSystemSettings.h"
#include "Module/ShopSystemModule.h"

void UShopManager::InitManager()
{
	Super::InitManager();

	ValidateAllowedItemsInCategories();	
}

bool UShopManager::PreConvertToSaveData_Implementation()
{
	ItemsSaveData.Empty();

	for(TTuple<UShopCategoryData*, FShopItemsList>& Pair : Items)
	{
		FShopItemsSaveDataList CategorySaveData;

		if(!USerializationSystemLibrary::ConvertObjectsToSaveData<UShopItem>(Pair.Value.Items, CategorySaveData.SaveData)) continue;

		ItemsSaveData.Add(Pair.Key, CategorySaveData);
	}

	return true;
}

bool UShopManager::PostConvertFromSaveData_Implementation()
{
	Items.Empty();

	for(TTuple<UShopCategoryData*, FShopItemsSaveDataList>& Pair : ItemsSaveData)
	{
		FShopItemsList CategoryItems;

		CategoryItems.Items = USerializationSystemLibrary::ConvertSaveDataToObjects<UShopItem>(Pair.Value.SaveData, this);

		Items.Add(Pair.Key, CategoryItems);
	}

	return true;
}

void UShopManager::ValidateAllowedItemsInCategories()
{
	for(TPair<UShopCategoryData*, FShopItemsList>& Pair : Items)
	{
		for(int i = Pair.Value.Items.Num() - 1; i >= 0; i--)
		{
			UShopItem* Item = Pair.Value.Items[i];

			if(!IsItemAllowedInCategory(Item, Pair.Key)) Pair.Value.Items.Remove(Item);
		}
	}
}

bool UShopManager::BuyItem(UShopItem* Item)
{
	if(!Item) return false;

	const bool Result = Item->CanBeBought();
	// Try Buy
	if(Result)
	{
		// Bind Item Delegates
		Item->OnItemBought.AddUniqueDynamic(this, &UShopManager::ProcessPurchaseBought);
		Item->OnItemBuyFailed.AddUniqueDynamic(this, &UShopManager::ProcessPurchaseFailed);
		Item->OnItemApplied.AddUniqueDynamic(this, &UShopManager::ProcessPurchaseApplied);
		Item->OnItemRefunded.AddUniqueDynamic(this, &UShopManager::ProcessPurchaseRefunded);

		DEBUG_MESSAGE(GetDefault<UShopSystemSettings>()->bShowDebugMessages, LogShopSystem, "Try To Buy %s", *Item->GetName())
		
		Item->Buy();
	}

	return Result;
}

bool UShopManager::AddItemToCategory(UShopItem* Item, UShopCategoryData* Category, bool bIgnoreAllowedClasses)
{
	if(!Item || !Category) return false;

	if(!IsItemAllowedInCategory(Item, Category)) return false;

	// Add To Category List
	FShopItemsList& CategoryItems = Items.FindOrAdd(Category);
	Item->Rename(nullptr, this);
	CategoryItems.Items.Add(Item);

	DEBUG_MESSAGE(GetDefault<UShopSystemSettings>()->bShowDebugMessages, LogShopSystem, "%s Added To %s", *Item->GetName(), *Category->GetName())

	OnCategoryUpdate.Broadcast(Category);

	return true;
}

bool UShopManager::RemoveItem(UShopItem* Item)
{
	if(!Item) return false;

	bool Removed = false;

	// Iterate Categories
	for(TTuple<UShopCategoryData*, FShopItemsList>& Pair : Items)
	{
		if(Pair.Value.Items.Remove(Item) > 0)
		{
			Removed = true;

			DEBUG_MESSAGE(GetDefault<UShopSystemSettings>()->bShowDebugMessages, LogShopSystem, "%s Removed From %s", *Item->GetName(), *Pair.Key->GetName())
			
			OnCategoryUpdate.Broadcast(Pair.Key);
		}
	}

	return Removed;
}

bool UShopManager::IsItemAllowedInCategory(UShopItem* Item, UShopCategoryData* Category)
{
	if(!Item || !Category) return false;

	if(Category->AllowedItemClasses.Num() > 0)
	{
		for(const TSubclassOf<UShopItem> AllowedClass : Category->AllowedItemClasses)
		{
			if(Item->GetClass()->IsChildOf(AllowedClass) || Item->GetClass() == AllowedClass) break;
		}

		return false;
	}

	if(Category->AllowedItemData.Num() > 0)
	{
		for(const UShopItemData* AllowedData : Category->AllowedItemData)
		{
			if(Item->GetShopData<UShopItemData>() == AllowedData) break;
		}

		return false;
	}

	return true;
}

TArray<UShopItem*> UShopManager::GetShopItems(TSubclassOf<UShopItem> Class) const
{
	TArray<UShopItem*> Out;

	// Iterate Shop Items
	for(const TTuple<UShopCategoryData*, FShopItemsList>& Pair : Items)
	{
		for(UShopItem* Item : Pair.Value.Items)
		{
			if(Item && Item->GetClass()->IsChildOf(Class)) Out.Add(Item);
		}
	}

	return Out;
}


TArray<UShopItem*> UShopManager::GetShopItemsWithTag(TSubclassOf<UShopItem> Class, FName Tag) const
{
	const TArray<UShopItem*> Out = GetShopItems(Class).FilterByPredicate([Tag](const UShopItem* Src)
	{
		// Check Tag
		return Src && Src->GetTag() == Tag;	
	});

	return Out;
}

TArray<UShopItem*> UShopManager::GetShopItemsInCategory(TSubclassOf<UShopItem> Class, UShopCategoryData* Category) const
{
	const TMap<UShopCategoryData*, FShopItemsList>& ItemsCopy = Items;

	const TArray<UShopItem*> Out = GetShopItems(Class).FilterByPredicate([ItemsCopy, Category](const UShopItem* Src)
	{
		// Check Category
		return Src && ItemsCopy.Contains(Category) && ItemsCopy.Find(Category)->Items.Contains(Src);
	});

	return Out;
}

TArray<UShopItem*> UShopManager::GetShopItemsWithTagInCategory(TSubclassOf<UShopItem> Class, UShopCategoryData* Category, FName Tag) const
{
	const TMap<UShopCategoryData*, FShopItemsList>& ItemsCopy = Items;

	const TArray<UShopItem*> Out = GetShopItems(Class).FilterByPredicate([ItemsCopy, Category, Tag](const UShopItem* Src)
	{
		// Check Category And Tag
		return Src && ItemsCopy.Contains(Category) && ItemsCopy.Find(Category)->Items.Contains(Src) && Src->GetTag() == Tag;
	});

	return Out;
}

UShopItem* UShopManager::GetShopItemWithID(TSubclassOf<UShopItem> Class, FString ID) const
{
	UShopItem* const* Out = GetShopItems(Class).FindByPredicate([ID](const UShopItem* Src)
	{
		// Check Unique ID
		return Src && Src->GetID() == ID;
	});

	return *Out;
}

void UShopManager::ProcessPurchaseBought(UShopItem* Item)
{
	// Unbind Item Delegates
	Item->OnItemBought.RemoveDynamic(this, &UShopManager::ProcessPurchaseBought);
	Item->OnItemBuyFailed.RemoveDynamic(this, &UShopManager::ProcessPurchaseFailed);
	Item->OnItemApplied.RemoveDynamic(this, &UShopManager::ProcessPurchaseApplied);
	Item->OnItemRefunded.RemoveDynamic(this, &UShopManager::ProcessPurchaseRefunded);

	OnItemBought.Broadcast(Item);
}

void UShopManager::ProcessPurchaseFailed(UShopItem* Item)
{
	// Unbind Item Delegates
	Item->OnItemBought.RemoveDynamic(this, &UShopManager::ProcessPurchaseBought);
	Item->OnItemBuyFailed.RemoveDynamic(this, &UShopManager::ProcessPurchaseFailed);
	Item->OnItemApplied.RemoveDynamic(this, &UShopManager::ProcessPurchaseApplied);
	Item->OnItemRefunded.RemoveDynamic(this, &UShopManager::ProcessPurchaseRefunded);

	OnItemBuyFailed.Broadcast(Item);
}

void UShopManager::ProcessPurchaseApplied(UShopItem* Item)
{
	// Unbind Item Delegates
	Item->OnItemApplied.RemoveDynamic(this, &UShopManager::ProcessPurchaseApplied);
	Item->OnItemRefunded.RemoveDynamic(this, &UShopManager::ProcessPurchaseRefunded);
	
	OnItemApplied.Broadcast(Item);
}

void UShopManager::ProcessPurchaseRefunded(UShopItem* Item)
{
	// Unbind Item Delegates
	Item->OnItemApplied.RemoveDynamic(this, &UShopManager::ProcessPurchaseApplied);
	Item->OnItemRefunded.RemoveDynamic(this, &UShopManager::ProcessPurchaseRefunded);
	
	OnItemRefunded.Broadcast(Item);
}