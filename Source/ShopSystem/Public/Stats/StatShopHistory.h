// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Stats/Stat.h"

#include "Items/ShopItem.h"

#include "StatShopHistory.generated.h"

class UShopItemData;
class UShopManager;

UENUM(BlueprintType)
enum EOperationType
{
	Purchased,
	Refunded
};

USTRUCT(BlueprintType)
struct SHOPSYSTEM_API FPurchaseData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shop|Purchases")
	TSubclassOf<UShopItem> ShopItemClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shop|Purchases")
	UShopItemData* ShopItem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shop|Purchases")
	FDateTime PurchaseTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shop|Purchases")
	TEnumAsByte<EOperationType> OperationType;

	FPurchaseData(TSubclassOf<UShopItem> ItemClass = UShopItem::StaticClass(),
		UShopItemData* Item = nullptr,
		FDateTime Time = FDateTime(),
		TEnumAsByte<EOperationType> Operation = EOperationType::Purchased) :
	ShopItemClass(ItemClass),
	ShopItem(Item),
	PurchaseTime(Time),
	OperationType(Operation)
	{
	}
};

USTRUCT(BlueprintType)
struct SHOPSYSTEM_API FPurchaseSaveData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shop|Purchases")
	TSoftClassPtr<UShopItem> ShopItemClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shop|Purchases")
	FName ShopItemTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shop|Purchases")
	FDateTime PurchaseTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shop|Purchases")
	TEnumAsByte<EOperationType> OperationType;

	FPurchaseSaveData(
		TSoftClassPtr<UShopItem> ItemClass = UShopItem::StaticClass(),
		FName Item = NAME_None,
		FDateTime Time = FDateTime(),
		TEnumAsByte<EOperationType> Operation = EOperationType::Purchased) :
	ShopItemClass(ItemClass),
	ShopItemTag(Item),
	PurchaseTime(Time),
	OperationType(Operation)
	{
	}
};

UCLASS()
class SHOPSYSTEM_API UStatShopHistory : public UStat
{
	GENERATED_BODY()

	friend UShopManager;

protected:

	UPROPERTY(EditAnywhere, Category = "Shop|Purchases")
	TArray<FPurchaseSaveData> Purchases;
	
public:

	UFUNCTION(BlueprintPure, Category = "Shop|Purchases", meta = (CompactNodeTitle = "Purchases", DeterminesOutputType = "Class"))
	TArray<FPurchaseData> GetAllPurchases(TSubclassOf<UShopItemData> Class) const;

	UFUNCTION(BlueprintPure, Category = "Shop|Purchases", meta = (CompactNodeTitle = "Purchases", DeterminesOutputType = "Class"))
	TArray<FPurchaseData> GetAllPurchasesInPeriod(TSubclassOf<UShopItemData> Class, FDateTime Start, FDateTime End) const;

	UFUNCTION(BlueprintPure, Category = "Shop|Purchases", meta = (CompactNodeTitle = "ToSaveData"))
	static FPurchaseSaveData ToPurchaseSaveData(FPurchaseData PurchaseData);

	UFUNCTION(BlueprintPure, Category = "Shop|Purchases", meta = (CompactNodeTitle = "ToSaveData", WorldContext = "WorldContext"))
	static FPurchaseData FromPurchaseSaveData(const FPurchaseSaveData PurchaseSaveData, const UObject* WorldContext);

protected:

	void RecordPurchase(UShopItem* ShopItem, TEnumAsByte<EOperationType> OperationType);
};
