// Copyright shenkns Shop System Developed With Unreal Engine. All Rights Reserved 2023.

#include "Items/ShopItem.h"

#include "ManagersSystem.h"

#include "Data/ShopItemData.h"
#include "Managers/ShopManager.h"
#include "Managers/StatsManager.h"
#include "Stats/CurrencyStat.h"

UWorld* UShopItem::GetWorld() const
{
	return GetOuter() ? GetOuter()->GetWorld() : nullptr;
}

void UShopItem::PostInitProperties()
{
	Super::PostInitProperties();

	// Generate Unique ID
	if(UniqueID.IsEmpty())
	{
		UniqueID = FGuid::NewGuid().ToString();
	}
}

void UShopItem::Init_Implementation()
{
	
}

UManagersSystem* UShopItem::GetManagersSystem() const
{
	return Cast<UManagersSystem>(GetOuter()->GetOuter());
}

UShopManager* UShopItem::GetManager() const
{
	return Cast<UShopManager>(GetOuter());
}

FName UShopItem::GetTag() const
{
	return ShopData ? ShopData->Tag : NAME_None;
}

UCurrencyData* UShopItem::GetCurrency_Implementation() const
{
	return ShopData ? ShopData->Currency : nullptr;
}

int UShopItem::GetPrice_Implementation() const
{
	return ShopData ? ShopData->Price : 0;
}

void UShopItem::Buy_Implementation()
{
	const UManagersSystem* ManagersSystem = GetManagersSystem();
	if(!ManagersSystem) return;

	UStatsManager* StatsManager = ManagersSystem->GetManager<UStatsManager>();
	if(!StatsManager) return;

	UCurrencyStat* CurrencyStat = StatsManager->GetStat<UCurrencyStat>();
	if(!CurrencyStat) return;
	
	FinishPurchase(CurrencyStat->ChangeCurrency(GetCurrency(), -GetPrice()));

	StatsManager->SaveStats();
}

void UShopItem::Finish_Implementation()
{
	return;
}

bool UShopItem::Apply_Implementation()
{
	return true;
}

bool UShopItem::Refund_Implementation()
{
	return true;
}


bool UShopItem::CanBeBought_Implementation() const
{
	if(!ShopData) return false;

	const UManagersSystem* ManagersSystem = GetManagersSystem();
	if(!ManagersSystem) return false;

	const UStatsManager* StatsManager = ManagersSystem->GetManager<UStatsManager>();
	if(!StatsManager) return false;

	const UCurrencyStat* CurrencyStat = StatsManager->GetStat<UCurrencyStat>();
	if(!CurrencyStat) return false;

	return CurrencyStat->HasCurrency(GetCurrency(), GetPrice());
}

void UShopItem::FinishPurchase(bool Result)
{
	// Apply Item
	if(Result)
	{
		const bool ApplyResult = ApplyPurchase();
		if(ApplyResult)
		{
			Finish();
			OnItemBought.Broadcast(this);
			return;
		}
	}
	
	OnItemBuyFailed.Broadcast(this);
}

bool UShopItem::ApplyPurchase()
{
	const bool Result = Apply();

	// Check Item Added
	if(Result)
	{
		OnItemApplied.Broadcast(this);
	}
	else
	{
		RefundPurchase(false);
	}

	return Result;
}

bool UShopItem::RefundPurchase(bool ItemGiven)
{
	bool Result = true;

	// Remove Item From Player
	if(ItemGiven)
	{
		Result = Refund();
	}

	// Refund Currency
	if(Result)
	{
		const UManagersSystem* ManagersSystem = GetManagersSystem();
		if(!ManagersSystem) return Result;

		const UStatsManager* StatsManager = ManagersSystem->GetManager<UStatsManager>();
		if(!StatsManager) return Result;

		UCurrencyStat* CurrencyStat = StatsManager->GetStat<UCurrencyStat>();
		if(!CurrencyStat) return Result;

		if(CurrencyStat->ChangeCurrency(GetCurrency(), GetPrice()))
		{
				OnItemRefunded.Broadcast(this);
		}
	}

	return Result;
}
