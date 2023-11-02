// Copyright shenkns Shop System Developed With Unreal Engine. All Rights Reserved 2023.

#pragma once

#include "Data/Data.h"

#include "ShopItemData.generated.h"

class UCurrencyData;

UCLASS()
class SHOPSYSTEM_API UShopItemData : public UData
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shop")
	UCurrencyData* Currency;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shop")
	int Price;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shop|Verification")
	bool bSkipPurchaseVerification;
};
