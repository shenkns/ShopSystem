// Copyright shenkns Shop System Developed With Unreal Engine. All Rights Reserved 2023.

#include "Items/ShopItem.h"

#include "HttpManager.h"
#include "HttpModule.h"
#include "ManagersSystem.h"

#include "Data/ShopItemData.h"
#include "Interfaces/IHttpResponse.h"
#include "Managers/ShopManager.h"
#include "Managers/StatsManager.h"
#include "Module/ShopSystemSettings.h"
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

FText UShopItem::GetPriceText_Implementation() const
{
	return ShopData ? FText::AsNumber(ShopData->Price) : FText();
}

UCurrencyData* UShopItem::GetCurrency_Implementation() const
{
	return ShopData ? ShopData->Currency : nullptr;
}

int UShopItem::GetPrice_Implementation() const
{
	return ShopData ? ShopData->Price : 0;
}

bool UShopItem::Buy_Implementation()
{
	const UManagersSystem* ManagersSystem = GetManagersSystem();
	if(!ManagersSystem) return false;

	UStatsManager* StatsManager = ManagersSystem->GetManager<UStatsManager>();
	if(!StatsManager) return false;

	UCurrencyStat* CurrencyStat = StatsManager->GetStat<UCurrencyStat>();
	if(!CurrencyStat) return false;

	const bool bSuccess = !GetCurrency() || CurrencyStat->ChangeCurrency(GetCurrency(), -GetPrice());
	
	if(bSuccess)
	{
		if(const UShopSystemSettings* Settings = GetDefault<UShopSystemSettings>())
		{
			if(Settings->bEnableBackendPurchaseVerification)
			{
				VerifyPurchase();
				return true;
			}
		}
	}

	FinishPurchase(bSuccess);
	
	StatsManager->SaveStats();

	return bSuccess;
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

void UShopItem::VerifyPurchase_Implementation()
{
	FHttpModule& HttpModule = FHttpModule::Get();
	
	const TSharedRef<IHttpRequest, ESPMode::ThreadSafe> VerificationRequest = HttpModule.CreateRequest();
	VerificationRequest->SetURL(UShopSystemSettings::GetPurchaseVerificationUrl());
	VerificationRequest->SetVerb(FString("POST"));

	VerificationRequest->SetHeader(FString("Authorization"),
		FString::Printf(TEXT("Bearer %s"), *UShopSystemSettings::GetBackendAuthToken())
	);

	const TSharedPtr<FJsonObject> RequestContent = MakeShareable(new FJsonObject);
	RequestContent->SetStringField(FString("tag"), GetShopData<UShopItemData>()->Tag.ToString());
	
	FString OutputString;
	const TSharedRef<TJsonWriter<TCHAR>> Writer = TJsonWriterFactory<TCHAR>::Create(&OutputString);
	FJsonSerializer::Serialize(RequestContent.ToSharedRef(), Writer);

	VerificationRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	VerificationRequest->SetContentAsString(OutputString);

	VerificationRequest->OnProcessRequestComplete().BindWeakLambda(this, [this](FHttpRequestPtr Request,
			FHttpResponsePtr Response,
			bool bConnectedSuccessfully)
	{
		OnPurchaseVerified(Response.Get()->GetResponseCode() == 200);
	});

	VerificationRequest->ProcessRequest();
}

void UShopItem::OnPurchaseVerified_Implementation(bool bSuccess)
{
	FinishPurchase(bSuccess);
}

void UShopItem::FinishPurchase(bool Result)
{
	// Apply Item
	if(Result)
	{
		const bool ApplyResult = ApplyPurchase();
		
		if(ApplyResult)
		{
			OnItemBought.Broadcast(this);
		}

		Finish();

		return;
	}
	
	OnItemBuyFailed.Broadcast(this);

	Finish();
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
