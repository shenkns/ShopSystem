#include "Module/ShopSystemSettings.h"

#include "Connection/BackendPathGetter.h"

FString UShopSystemSettings::GetPurchaseVerificationUrl()
{
	UShopSystemSettings* Settings = GetMutableDefault<UShopSystemSettings>();
	
	const UBackendPathGetter* BackendPathGetter = NewObject<UBackendPathGetter>(Settings, Settings->BackendPurchaseVerificationPathGetterClass);
	if(!BackendPathGetter) return FString();

	return BackendPathGetter->GetBackendPathUrl();
}
