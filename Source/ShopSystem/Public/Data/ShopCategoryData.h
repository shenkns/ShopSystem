// Copyright shenkns Shop System Developed With Unreal Engine. All Rights Reserved 2023.

#pragma once

#include "Data/Data.h"

#include "ShopCategoryData.generated.h"

class UShopItem;
class UShopItemData;

UCLASS()
class SHOPSYSTEM_API UShopCategoryData : public UData
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shop")
	TArray<TSubclassOf<UShopItem>> AllowedItemClasses;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shop")
	TArray<UShopItemData*> AllowedItemData;
};
