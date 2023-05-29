// Fill out your copyright notice in the Description page of Project Settings.

#include "Stats/StatShopHistory.h"

#include "ManagersSystem.h"
#include "Data/ShopItemData.h"
#include "Managers/DataManager.h"

TArray<FPurchaseData> UStatShopHistory::GetAllPurchases(TSubclassOf<UShopItemData> Class) const
{
	const UManagersSystem* ManagersSystem = GetManagersSystem();
	if(!ManagersSystem) return TArray<FPurchaseData>();

	const UDataManager* DataManager = ManagersSystem->GetManager<UDataManager>();
	if(!DataManager) return TArray<FPurchaseData>();

	TArray<FPurchaseData> AllPurchases;

	for(const FPurchaseSaveData PurchaseSaveData : Purchases)
	{
		FPurchaseData Purchase = FromPurchaseSaveData(PurchaseSaveData, this);
		if(!Purchase.ShopItem) continue;

		if(Purchase.ShopItem->GetClass()->IsChildOf(Class))
		{
			AllPurchases.Add(Purchase);
		}
	}

	return AllPurchases;
}

TArray<FPurchaseData> UStatShopHistory::GetAllPurchasesInPeriod(TSubclassOf<UShopItemData> Class, FDateTime Start, FDateTime End) const
{
	const UManagersSystem* ManagersSystem = GetManagersSystem();
	if(!ManagersSystem) return TArray<FPurchaseData>();

	const UDataManager* DataManager = ManagersSystem->GetManager<UDataManager>();
	if(!DataManager) return TArray<FPurchaseData>();

	TArray<FPurchaseData> AllPurchases;

	for(const FPurchaseSaveData PurchaseSaveData : Purchases)
	{
		FPurchaseData Purchase = FromPurchaseSaveData(PurchaseSaveData, this);
		if(!Purchase.ShopItem) continue;

		if(Purchase.ShopItem->GetClass()->IsChildOf(Class))
		{
			if(Purchase.PurchaseTime >= Start && Purchase.PurchaseTime <= End)
			{
				AllPurchases.Add(Purchase);
			}
		}
	}

	return AllPurchases;
}

FPurchaseSaveData UStatShopHistory::ToPurchaseSaveData(FPurchaseData PurchaseData)
{
	return FPurchaseSaveData(PurchaseData.ShopItem ? PurchaseData.ShopItem->Tag : NAME_None,
		PurchaseData.PurchaseTime,
		PurchaseData.OperationType
	);
}

FPurchaseData UStatShopHistory::FromPurchaseSaveData(const FPurchaseSaveData PurchaseSaveData, const UObject* WorldContext)
{
	const UManagersSystem* ManagersSystem = UManagersSystem::Get(WorldContext);
	if(!ManagersSystem) return FPurchaseData(nullptr,
		PurchaseSaveData.PurchaseTime,
		PurchaseSaveData.OperationType
	);

	const UDataManager* DataManager = ManagersSystem->GetManager<UDataManager>();
	if(!DataManager) return FPurchaseData(nullptr,
		PurchaseSaveData.PurchaseTime,
		PurchaseSaveData.OperationType
	);
	
	return FPurchaseData(DataManager->GetDataAsset<UShopItemData>(PurchaseSaveData.ShopItemTag),
		PurchaseSaveData.PurchaseTime,
		PurchaseSaveData.OperationType
	);
}

void UStatShopHistory::RecordPurchase(UShopItemData* ShopItem, TEnumAsByte<EOperationType> OperationType)
{
	Purchases.Add(ToPurchaseSaveData(FPurchaseData(ShopItem, FDateTime::UtcNow(), OperationType)));
}
