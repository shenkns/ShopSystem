#include "Module/ShopSystemSettings.h"

#include "Connection\StringObjectGetter.h"

FString UShopSystemSettings::GetPurchaseVerificationUrl()
{
	UShopSystemSettings* Settings = GetMutableDefault<UShopSystemSettings>();

	if(!Settings->BackendPurchaseVerificationPathGetterClass) return FString();
	
	const UStringObjectGetter* BackendPathGetter = NewObject<UStringObjectGetter>(Settings, Settings->BackendPurchaseVerificationPathGetterClass);
	if(!BackendPathGetter) return FString();

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
