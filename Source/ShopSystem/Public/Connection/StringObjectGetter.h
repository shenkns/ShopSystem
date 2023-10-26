// Copyright shenkns Shop System Developed With Unreal Engine. All Rights Reserved 2023.

#pragma once

#include "UObject/Object.h"

#include "StringObjectGetter.generated.h"

UCLASS(Abstract, BlueprintType, Blueprintable)
class SHOPSYSTEM_API UStringObjectGetter : public UObject
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintPure, BlueprintNativeEvent, Category = "Url")
	FString GetString() const;
};
