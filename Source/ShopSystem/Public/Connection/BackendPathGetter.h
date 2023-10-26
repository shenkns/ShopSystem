// Copyright shenkns Shop System Developed With Unreal Engine. All Rights Reserved 2023.

#pragma once

#include "UObject/Object.h"

#include "BackendPathGetter.generated.h"

UCLASS(Abstract, BlueprintType, Blueprintable)
class SHOPSYSTEM_API UBackendPathGetter : public UObject
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintPure, BlueprintNativeEvent, Category = "Url")
	FString GetBackendPathUrl() const;
};
