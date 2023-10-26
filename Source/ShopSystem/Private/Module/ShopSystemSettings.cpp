#include "Module/ShopSystemSettings.h"

#include "Connection\StringObjectGetter.h"

FString UShopSystemSettings::GetPurchaseVerificationUrl()
{
	UShopSystemSettings* Settings = GetMutableDefault<UShopSystemSettings>();
	
	const UStringObjectGetter* BackendPathGetter = NewObject<UStringObjectGetter>(Settings, Settings->BackendPurchaseVerificationPathGetterClass);
	if(!BackendPathGetter) return FString();

	return BackendPathGetter->GetString();
}
