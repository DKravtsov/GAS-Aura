// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Inventory/Spatial/InventoryWidgetSpatial.h"
#include "InventoryStoreWidgetSpatial.generated.h"

/**
 * 
 */
UCLASS(MinimalAPI, Abstract)
class UInventoryStoreWidgetSpatial : public UInventoryWidgetSpatial
{
	GENERATED_BODY()

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UInventoryGridWidget> StoreGrid_Equipment;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UInventoryGridWidget> StoreGrid_Consumable;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UInventoryGridWidget> StoreGrid_Crafting;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UWidgetSwitcher> StoreGridSwitcher;

	TWeakObjectPtr<UInventoryStoreComponent> StoreComponent;

public:

	//~ Begin of UUserWidget interface
	virtual bool Initialize() override;
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	//~ End of UUserWidget interface

	virtual void OnOpenedMenu() override;

	void PopulateStore(UInventoryStoreComponent* Store);

	UFUNCTION(BlueprintPure, Category = "Inventory")
	UInventoryStoreComponent* GetStoreComponent() const {return StoreComponent.Get();}

protected:

	virtual void UpdateInventoryGrids() override;

private:
	void HandleOnActiveGridSwitched(UInventoryGridWidget* InventoryGridWidget);

	void PopulateItemsFromStore();
};
