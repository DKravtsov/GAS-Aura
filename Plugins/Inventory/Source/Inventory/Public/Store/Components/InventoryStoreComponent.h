// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "InventoryGridTypes.h"
#include "InventoryManagement/Components/InventoryManagementComponentBase.h"
#include "InventoryManagement/FastArray/InventoryFastArray.h"
#include "InventoryStoreComponent.generated.h"


class UInventoryStorage;
class UInventoryWidgetBase;

UCLASS(MinimalAPI, ClassGroup=(Inventory), meta=(BlueprintSpawnableComponent), Blueprintable)
class UInventoryStoreComponent : public UInventoryManagementComponentBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Inventory")
	FText InteractMessage;

	// we need this to track whether the menu is open on one of the clients to prevent open it on another one.
	uint8 bInventoryMenuOpen:1 = false;

	uint8 bStartupItemsInitialized:1 = false;

public:

	UInventoryStoreComponent();

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	const FText& GetInteractMessage() const { return InteractMessage; }

	bool IsMenuOpen() const { return bInventoryMenuOpen; }
	void SetMenuOpen(bool bOpen) { bInventoryMenuOpen = bOpen; }
	
	int32 GetSellValue(const UInventoryItem* Item, int32 StackCount) const;
	bool HasCoins(int32 SellValue);
	void RemoveCoins(int32 SellValue);
	void TryAddItem(const FInventoryItemManifest& ItemManifest, int32 StackCount);

protected:

	virtual void BeginPlay() override;

private:


};
