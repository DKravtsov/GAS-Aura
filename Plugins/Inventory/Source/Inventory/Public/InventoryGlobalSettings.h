// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "InventoryGlobalSettings.generated.h"

struct FInventoryItemManifest;
class UInventoryItemData;
class UInventoryStorageGrid;
/**
 * 
 */
UCLASS()
class INVENTORY_API UInventoryGlobalSettings : public UDeveloperSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSoftClassPtr<UInventoryStorageGrid> SpatialStorageGridClass;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSoftObjectPtr<UInventoryItemData> CoinsItemData;

public:

	UInventoryGlobalSettings(const FObjectInitializer& ObjectInitializer=FObjectInitializer::Get());

	static const UInventoryGlobalSettings& Get(); 

	static TSubclassOf<UInventoryStorageGrid> GetSpatialStorageGridClass();

	static const FInventoryItemManifest* GetCoinsItemManifest();
};
