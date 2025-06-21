// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "InventoryStatics.generated.h"

class UInventoryItemComponent;
class UInventoryComponent;
/**
 * 
 */
UCLASS()
class UInventoryStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	static INVENTORY_API UInventoryComponent* GetInventoryComponent(const APlayerController* PlayerController);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	static INVENTORY_API UInventoryItemComponent* GetInventoryItemComponent(const AActor* Actor);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	static INVENTORY_API FGameplayTag GetItemCategory(UInventoryItemComponent* ItemComponent);
};
