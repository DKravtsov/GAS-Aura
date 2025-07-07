// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "InventoryGlobalSettings.generated.h"

class UInventorySpatialStorageGrid;
/**
 * 
 */
UCLASS()
class INVENTORY_API UInventoryGlobalSettings : public UDeveloperSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSoftClassPtr<UInventorySpatialStorageGrid> SpatialStorageGridClass;

public:

	UInventoryGlobalSettings(const FObjectInitializer& ObjectInitializer=FObjectInitializer::Get());

	static const UInventoryGlobalSettings& Get(); 

	static TSubclassOf<UInventorySpatialStorageGrid> GetSpatialStorageGridClass();
};
