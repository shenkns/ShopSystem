// Copyright shenkns Shop System Developed With Unreal Engine. All Rights Reserved 2023.

#pragma once

#include "UObject/Object.h"

#include "ShopSystemSettings.generated.h"

UCLASS(Config=Game, DefaultConfig)
class SHOPSYSTEM_API UShopSystemSettings : public UObject
{
	GENERATED_BODY()

public:

	// Backend verification
	UPROPERTY(EditDefaultsOnly, Config, Category = "Backend")
	bool bEnableBackendPurchaseVerification;

	UPROPERTY(EditDefaultsOnly, Config, Category = "Backend", meta = (EditCondition = "bEnableBackendPurchaseVerification"))
	FString BackendPurchaseVerificationUrl;

	// Debug
	UPROPERTY(EditDefaultsOnly, Config, Category = "Debug")
	bool bShowDebugMessages;
};