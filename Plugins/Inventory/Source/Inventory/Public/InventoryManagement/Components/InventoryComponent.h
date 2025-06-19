// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"


class UInventoryWidgetBase;

UCLASS(MinimalAPI, ClassGroup=(Inventory), Blueprintable, Abstract, meta=(BlueprintSpawnableComponent))
class UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSoftClassPtr<UInventoryWidgetBase> InventoryMenuClass;

	UPROPERTY(Transient)
	TObjectPtr<UInventoryWidgetBase> InventoryMenu;

	TWeakObjectPtr<APlayerController> OwningPlayerController;
	
public:
	INVENTORY_API UInventoryComponent();

protected:
	INVENTORY_API virtual void BeginPlay() override;

private:

	void ConstructInventory();
};
