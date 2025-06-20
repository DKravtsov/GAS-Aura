// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

class UInventoryItemComponent;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInventoryItemChangeSignature, UInventoryItem*, Item);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FInventoryHasNoRoomSignature);

class UInventoryWidgetBase;

UCLASS(MinimalAPI, ClassGroup=(Inventory), Blueprintable, Abstract, meta=(BlueprintSpawnableComponent))
class UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	FInventoryItemChangeSignature OnItemAdded;
	FInventoryItemChangeSignature OnItemRemoved;
	FInventoryHasNoRoomSignature OnNoRoomInInventory;

private:
	
	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSoftClassPtr<UInventoryWidgetBase> InventoryMenuClass;

	UPROPERTY(Transient)
	TObjectPtr<UInventoryWidgetBase> InventoryMenu;

	TWeakObjectPtr<APlayerController> OwningPlayerController;
	TWeakObjectPtr<class UInventoryPlayerControllerComponent> InventoryController;

	bool bInventoryMenuOpen = false;
	
public:
	INVENTORY_API UInventoryComponent();

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	INVENTORY_API void ToggleInventoryMenu();

	UFUNCTION(BlueprintCallable, Category = "Inventory", BlueprintAuthorityOnly)
	INVENTORY_API void TryAddItem(UInventoryItemComponent* ItemComponent);

protected:
	INVENTORY_API virtual void BeginPlay() override;

private:

	void ConstructInventory();
	void OpenInventoryMenu();
	void CloseInventoryMenu();
};
