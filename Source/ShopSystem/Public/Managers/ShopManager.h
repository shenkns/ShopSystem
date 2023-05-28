// Copyright shenkns Shop System Developed With Unreal Engine. All Rights Reserved 2023.

#pragma once

#include "Managers/Manager.h"

#include "Items/ShopItem.h"
#include "Libs/SerializationSystemLibrary.h"

#include "ShopManager.generated.h"

class UShopCategoryData;

USTRUCT(BlueprintType)
struct SHOPSYSTEM_API FShopItemsList
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced, Category = "Shop|Items")
	TArray<UShopItem*> Items;
};

USTRUCT(BlueprintType)
struct SHOPSYSTEM_API FShopItemsSaveDataList
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shop|SaveData")
	TArray<FObjectSaveData> SaveData;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCategoryUpdateEvent, UShopCategoryData*, Category);


UCLASS()
class SHOPSYSTEM_API UShopManager : public UManager, public ISerializationInterface
{
	GENERATED_BODY()

	public:

	// Item Delegates
	UPROPERTY(BlueprintAssignable, Category = "Shop")
	FItemPurchaseEvent OnItemBought;

	UPROPERTY(BlueprintAssignable, Category = "Shop")
	FItemPurchaseEvent OnItemBuyFailed;

	UPROPERTY(BlueprintAssignable, Category = "Shop")
	FItemPurchaseEvent OnItemApplied;

	UPROPERTY(BlueprintAssignable, Category = "Shop")
	FItemPurchaseEvent OnItemRefunded;

	// Category Delegates
	UPROPERTY(BlueprintAssignable, Category = "Shop")
	FCategoryUpdateEvent OnCategoryUpdate;

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Shop")
	TMap<UShopCategoryData*, FShopItemsList> Items;

	UPROPERTY()
	TMap<UShopCategoryData*, FShopItemsSaveDataList> ItemsSaveData;

public:

	virtual void InitManager() override;

	UFUNCTION(BlueprintCallable, Category = "Shop")
	bool BuyItem(UShopItem* Item);

	// Manage Categories
	UFUNCTION(BlueprintCallable, Category = "Shop", meta = (CompactNodeTitle = "Add"))
	bool AddItemToCategory(UShopItem* Item, UShopCategoryData* Category, bool bIgnoreAllowedClasses = false);

	UFUNCTION(BlueprintCallable, Category = "Shop", meta = (CompactNodeTitle = "Remove"))
	bool RemoveItem(UShopItem* Item);
	
	// Category Getters
	UFUNCTION(BlueprintPure, Category = "Shop", meta = (CompactNodeTitle = "Allowed"))
	static bool IsItemAllowedInCategory(UShopItem* Item, UShopCategoryData* Category);

	// Items Getters
	UFUNCTION(BlueprintPure, Category = "Shop", meta = (CompactNodeTitle = "Items", DeterminesOutputType = "Class"))
	TArray<UShopItem*> GetShopItems(TSubclassOf<UShopItem> Class) const;

	UFUNCTION(BlueprintPure, Category = "Shop", meta = (CompactNodeTitle = "Items", DeterminesOutputType = "Class"))
	TArray<UShopItem*> GetShopItemsWithTag(TSubclassOf<UShopItem> Class, FName Tag) const;

	UFUNCTION(BlueprintPure, Category = "Shop", meta = (CompactNodeTitle = "Items", DeterminesOutputType = "Class"))
	TArray<UShopItem*> GetShopItemsInCategory(TSubclassOf<UShopItem> Class, UShopCategoryData* Category) const;

	UFUNCTION(BlueprintPure, Category = "Shop", meta = (CompactNodeTitle = "Items", DeterminesOutputType = "Class"))
	TArray<UShopItem*> GetShopItemsWithTagInCategory(TSubclassOf<UShopItem> Class, UShopCategoryData* Category, FName Tag) const;

	UFUNCTION(BlueprintPure, Category = "Shop", meta = (CompactNodeTitle = "Item", DeterminesOutputType = "Class"))
	UShopItem* GetShopItemWithID(TSubclassOf<UShopItem> Class, FString ID) const;

	template<typename T>
	TArray<T*> GetShopItems() const;

	template<typename T>
	TArray<T*> GetShopItemsWithTag(FName Tag) const;

	template<typename T>
	TArray<T*> GetShopItemsInCategory(UShopCategoryData* Category) const;

	template<typename T>
	TArray<T*> GetShopItemsWithTagInCategory(UShopCategoryData* Category, FName Tag) const;

	template<typename T>
	T* GetShopItemWithID(FString ID) const;

	// Serialization
	virtual bool PreConvertToSaveData_Implementation() override;
	virtual bool PostConvertFromSaveData_Implementation() override;

protected:

	// Manage Categories
	void ValidateAllowedItemsInCategories();
	
	// Purchase Process
	UFUNCTION()
	void ProcessPurchaseBought(UShopItem* Item);

	UFUNCTION()
	void ProcessPurchaseFailed(UShopItem* Item);

	UFUNCTION()
	void ProcessPurchaseApplied(UShopItem* Item);

	UFUNCTION()
	void ProcessPurchaseRefunded(UShopItem* Item);
};

template <typename T>
TArray<T*> UShopManager::GetShopItems() const
{
	TArray<UShopItem*> Out;

	// Iterate Shop Items
	for(const TTuple<UShopCategoryData*, FShopItemsList>& Pair : Items)
	{
		for(UShopItem* Item : Pair.Value.Items)
		{
			if(Item && Item->GetClass()->IsChildOf(T::StaticClass())) Out.Add(Item);
		}
	}

	return reinterpret_cast<TArray<T*>&>(Out);
}

template <typename T>
TArray<T*> UShopManager::GetShopItemsWithTag(FName Tag) const
{
	// Filter Shop Items
	const TArray<UShopItem*> Out = GetShopItems<UShopItem>().FilterByPredicate([Tag](const UShopItem* Src)
	{
		return Src && Src->GetClass()->IsChildOf(T::StaticClass()) && Src->GetShopData<UShopCategoryData>() && Src->GetTag() == Tag;
	});

	return reinterpret_cast<TArray<T*>&>(Out);
}

template <typename T>
TArray<T*> UShopManager::GetShopItemsInCategory(UShopCategoryData* Category) const
{
	// Filter Shop Items
	const TArray<UShopItem*> Out = GetShopItems<UShopItem>().FilterByPredicate([&](const UShopItem* Src)
	{
		return Src && Src->GetClass()->IsChildOf(T::StaticClass()) && Items.Contains(Category) && Items.Find(Category)->Items.Contains(Src);
	});

	return reinterpret_cast<TArray<T*>&>(Out);
}

template <typename T>
TArray<T*> UShopManager::GetShopItemsWithTagInCategory(UShopCategoryData* Category, FName Tag) const
{
	// Filter Shop Items
	const TArray<UShopItem*> Out = GetShopItems<UShopItem>().FilterByPredicate([&](const UShopItem* Src)
	{
		return Src && Src->GetClass()->IsChildOf(T::StaticClass()) && Items.Contains(Category) && Items.Find(Category)->Items.Contains(Src) && Src->GetTag() == Tag;
	});

	return reinterpret_cast<TArray<T*>&>(Out);
}

template <typename T>
T* UShopManager::GetShopItemWithID(FString ID) const
{
	// Find Shop Item
	UShopItem* const* Out = GetShopItems<UShopItem>().FindByPredicate([ID](const UShopItem* Src)
	{
		return Src && Src->GetClass()->IsChildOf(T::StaticClass()) && Src->GetID() == ID;
	});

	return Out ? static_cast<T*>(*Out) : nullptr;
}
