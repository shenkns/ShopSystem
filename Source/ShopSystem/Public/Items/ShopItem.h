// Copyright shenkns Shop System Developed With Unreal Engine. All Rights Reserved 2023.

#pragma once

#include "UObject/Object.h"

#include "ShopItem.generated.h"

class UManagersSystem;
class UShopManager;
class UShopItemData;
class UShopItem;
class UCurrencyData;
class UVaRestSubsystem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FItemPurchaseEvent, UShopItem*, Item);

UCLASS(BlueprintType, Blueprintable, EditInlineNew, DefaultToInstanced)
class SHOPSYSTEM_API UShopItem : public UObject
{
	GENERATED_BODY()

	public:

	// Purchase Delegates
	UPROPERTY(BlueprintAssignable, Category = "Shop")
	FItemPurchaseEvent OnItemBought;

	UPROPERTY(BlueprintAssignable, Category = "Shop")
	FItemPurchaseEvent OnItemBuyFailed;

	UPROPERTY(BlueprintAssignable, Category = "Shop")
	FItemPurchaseEvent OnItemApplied;

	UPROPERTY(BlueprintAssignable, Category = "Shop")
	FItemPurchaseEvent OnItemRefunded;
	
protected:

	// Item Data
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shop", meta = (ExposeOnSpawn = true))
	UShopItemData* ShopData;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shop", meta = (ExposeOnSpawn = true))
	FString UniqueID;

public:

	virtual UWorld* GetWorld() const override;

	virtual void PostInitProperties() override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Shop")
	void Init();

	UFUNCTION(BlueprintPure, Category = "Managers", meta = (CompactNodeTitle = "System"))
	UManagersSystem* GetManagersSystem() const;

	UFUNCTION(BlueprintPure, Category = "Managers", meta = (CompactNodeTitle = "Manager"))
	UShopManager* GetManager() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Shop")
	bool Buy();

	// Item Stats Getters
	UFUNCTION(BlueprintPure, Category = "Shop", meta = (CompactNodeTitle = "Data", DeterminesOutputType = "Class"))
	UShopItemData* GetShopData(TSubclassOf<UShopItemData> Class) const { return ShopData; }

	UFUNCTION(BlueprintPure, Category = "Profile|Shop", meta = (CompactNodeTitle = "Tag"))
	FName GetTag() const;

	UFUNCTION(BlueprintPure, Category = "Profile|Shop", meta = (CompactNodeTitle = "ID"))
	FString GetID() const { return UniqueID; };

	UFUNCTION(BlueprintPure, BlueprintNativeEvent, Category = "Shop", meta = (CompactNodeTitle = "Currency"))
	UCurrencyData* GetCurrency() const;

	UFUNCTION(BlueprintPure, BlueprintNativeEvent, Category = "Shop", meta = (CompactNodeTitle = "Price"))
	int GetPrice() const;

	UFUNCTION(BlueprintPure, BlueprintNativeEvent, Category = "Shop", meta = (CompactNodeTitle = "PriceText"))
	FText GetPriceText() const;
	
	UFUNCTION(BlueprintPure, BlueprintNativeEvent, Category = "Shop")
	bool CanBeBought() const;

	template<typename T>
	T* GetShopData() const;

protected:

	// Purchase Progress Events
	UFUNCTION(BlueprintNativeEvent, Category = "Shop")
	void Finish();

	UFUNCTION(BlueprintNativeEvent, Category = "Shop")
	bool Apply();

	UFUNCTION(BlueprintNativeEvent, Category = "Shop")
	bool Refund();

	UFUNCTION(BlueprintNativeEvent, Category = "Shop")
	void VerifyPurchase(const FString& TransactionID = "");

	UFUNCTION(BlueprintNativeEvent, Category = "Shop")
	void OnPurchaseVerified(UVaRestRequestJSON* Request);

	UFUNCTION(BlueprintCallable, Category = "Shop")
	void FinishPurchase(bool Result);
	
	bool ApplyPurchase();
	bool RefundPurchase(bool ItemGiven);

	UVaRestSubsystem* GetVaRest() const;
};

template <typename T>
T* UShopItem::GetShopData() const
{
	return ShopData ? static_cast<T*>(ShopData) : nullptr;
}
