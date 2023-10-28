// Copyright shenkns Shop System Developed With Unreal Engine. All Rights Reserved 2023.

#pragma once

#include "UObject/Object.h"

#include "ShopSystemSettings.generated.h"

class UStringObjectGetter;

UCLASS(Config=Game, DefaultConfig)
class SHOPSYSTEM_API UShopSystemSettings : public UObject
{
	GENERATED_BODY()

public:

	// Backend verification
	UPROPERTY(EditDefaultsOnly, Config, Category = "Backend")
	bool bEnableBackendPurchaseVerification;

	UPROPERTY(EditDefaultsOnly, Config, Category = "Backend", meta = (EditCondition = "bEnableBackendPurchaseVerification"))
	TSubclassOf<UStringObjectGetter> BackendPurchaseVerificationPathGetterClass;

	UPROPERTY(EditDefaultsOnly, Config, Category = "Backend", meta = (EditCondition = "bEnableBackendPurchaseVerification"))
	TSubclassOf<UStringObjectGetter> BackendAuthTokenGetterClass;

	// Debug
	UPROPERTY(EditDefaultsOnly, Config, Category = "Debug")
	bool bShowDebugMessages;

	UPROPERTY(EditDefaultsOnly, Config, Category = "Debug")
	TSubclassOf<UPurchaseWidget> PurchaseWidgetClass;

public:

	UFUNCTION(BlueprintPure, Category = "Backend")
	static FString GetPurchaseVerificationUrl();

	UFUNCTION(BlueprintPure, Category = "Backend")
	static FString GetBackendAuthToken();
};
