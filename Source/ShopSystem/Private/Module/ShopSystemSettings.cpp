#include "Module/ShopSystemSettings.h"

#include "LogSystem.h"
#include "Connection\StringObjectGetter.h"
#include "Kismet/GameplayStatics.h"
#include "Module/ShopSystemModule.h"

FString UShopSystemSettings::GetPurchaseVerificationUrl()
{
	UShopSystemSettings* Settings = GetMutableDefault<UShopSystemSettings>();

	if(!Settings->BackendPurchaseVerificationPathGetterClass)
	{
		GEngine->AddOnScreenDebugMessage(-1,5.0f, FColor::Emerald, "Invalid purchase verification url getter class"); 
		
		return FString();
	}
	
	const UStringObjectGetter* BackendPathGetter = NewObject<UStringObjectGetter>(Settings, Settings->BackendPurchaseVerificationPathGetterClass);
	if(!BackendPathGetter)
	{
		GEngine->AddOnScreenDebugMessage(-1,5.0f, FColor::Emerald, "Invalid purchase verification getter object"); 
		
		return FString();
	}
	
	return BackendPathGetter->GetString();
}

FString UShopSystemSettings::GetBackendAuthToken()
{
	UShopSystemSettings* Settings = GetMutableDefault<UShopSystemSettings>();

	if(!Settings->BackendAuthTokenGetterClass) return FString();
	
	const UStringObjectGetter* BackendPathGetter = NewObject<UStringObjectGetter>(Settings, Settings->BackendAuthTokenGetterClass);
	if(!BackendPathGetter) return FString();

	return BackendPathGetter->GetString();
}
