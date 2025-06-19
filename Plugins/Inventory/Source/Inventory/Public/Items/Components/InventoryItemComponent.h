// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryItemComponent.generated.h"


UCLASS(ClassGroup=(Inventory), Blueprintable, meta=(BlueprintSpawnableComponent))
class UInventoryItemComponent : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	FText PickupMessage;
	
public:

	INVENTORY_API UInventoryItemComponent();

	FText GetPickupMessage() { return PickupMessage; }
};
