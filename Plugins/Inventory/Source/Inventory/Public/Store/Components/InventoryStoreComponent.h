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

	UPROPERTY(ReplicatedUsing=OnRep_StartupItemsInitialized)
	uint8 bStartupItemsInitialized:1 = false;

public:

	UInventoryStoreComponent();

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	const FText& GetInteractMessage() const { return InteractMessage; }

	bool IsMenuOpen() const { return bInventoryMenuOpen; }
	void SetMenuOpen(bool bOpen) { bInventoryMenuOpen = bOpen; }

	bool AreStartupItemsInitialized() const { return bStartupItemsInitialized; }

	// The Sell Value is the price what the store is agree to pay for the item, not its actual value
	int32 GetSellValue(const UInventoryItem* Item, int32 StackCount) const;
	// The Buy Value is the price what the store wants for the item, not its actual value
	int32 GetPurchaseValue(const UInventoryItem* Item, int32 StackCount) const;

	//#if UE_WITH_CHEAT_MANAGER
	INVENTORY_API virtual void DebugPrintStorage() const override;
	//#endif//UE_WITH_CHEAT_MANAGER

	bool IsValidItem(UInventoryItem* Item, int32 GridIndex, int32 StackCount);

protected:

	virtual void BeginPlay() override;

	void AddStartupItems();
	
	UFUNCTION()
	virtual void StartupItemsAssetsLoaded();

	UFUNCTION()
	void OnRep_StartupItemsInitialized();

private:


};
