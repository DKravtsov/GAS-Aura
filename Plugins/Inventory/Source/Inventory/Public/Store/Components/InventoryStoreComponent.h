// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryManagement/FastArray/InventoryFastArray.h"
#include "InventoryStoreComponent.generated.h"



class UInventoryWidgetBase;

UCLASS(MinimalAPI, ClassGroup=(Inventory), meta=(BlueprintSpawnableComponent), Blueprintable)
class UInventoryStoreComponent : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSoftClassPtr<UInventoryWidgetBase> InventoryMenuClass;

	UPROPERTY(Transient)
	TObjectPtr<UInventoryWidgetBase> InventoryMenu;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	FText InteractMessage;

	UPROPERTY(Replicated)
	FInventoryFastArray InventoryList;

	uint8 bInventoryMenuOpen:1 = false;

	uint8 bStartupItemsInitialized:1 = false;

public:

	UInventoryStoreComponent();

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	UInventoryWidgetBase* GetInventoryMenu() const { return InventoryMenu; }
	const FText& GetInteractMessage() const { return InteractMessage; }

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	INVENTORY_API void ToggleInventoryMenu();

	bool IsMenuOpen() const { return bInventoryMenuOpen; }
	
	int32 GetSellValue(const UInventoryItem* Item, int32 StackCount) const;
	bool HasCoins(int32 SellValue);
	void RemoveCoins(int32 SellValue);
	void TryAddItem(const FInventoryItemManifest& ItemManifest, int32 StackCount);

protected:

	virtual void BeginPlay() override;

private:

	void ConstructInventory();
	void OpenInventoryMenu();
	void CloseInventoryMenu();

};
