// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryHUDWidget.generated.h"

class UInventoryInfoMessage;

UCLASS(MinimalAPI)
class UInventoryHUDWidget : public UUserWidget
{
	GENERATED_BODY()


	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UInventoryInfoMessage> InfoMessage;
	
public:

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	INVENTORY_API virtual void ShowPickupMessage(const FText& Message);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	INVENTORY_API virtual void HidePickupMessage();

	//~ Begin of UUserWidget interface
	INVENTORY_API virtual void NativeOnInitialized() override;
	//~ End of UUserWidget interface

protected:

	UFUNCTION(BlueprintImplementableEvent, Category = "Inventory", meta=(DisaplyName="OnShowPickupMessage"))
	INVENTORY_API void BP_ShowPickupMessage(const FText& Message);

	UFUNCTION(BlueprintImplementableEvent, Category = "Inventory", meta=(DisaplyName="OnHidePickupMessage"))
	INVENTORY_API void BP_HidePickupMessage();

	UFUNCTION()
	void OnInventoryHasNoRoom();
	
};
