// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "InventoryGridTypes.h"
#include "Engine/DataAsset.h"
#include "StructUtils/InstancedStruct.h"
#include "InventorySetupData.generated.h"

class UInventoryItemData;
class UInventoryStorage;
struct FInventoryStorageSetupData;

USTRUCT(BlueprintType)
struct FInventoryItemProxy
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSoftObjectPtr<UInventoryItemData> InventoryItem;

	UPROPERTY(EditAnywhere, Category = "Inventory", meta=(EditCondition="bOverrideCount"))
	FIntPoint MinMaxAmount {1,1};

	UPROPERTY(EditAnywhere, Category = "Inventory", meta=(EditCondition="bShouldEquip"), DisplayName="Equip to slot")
	EInventoryEquipmentSlot EquipmentSlot = EInventoryEquipmentSlot::Invalid;

	UPROPERTY(EditAnywhere, Category = "Inventory", meta=(InlineEditConditionToggle))
	uint8 bOverrideCount:1 = false;

	UPROPERTY(EditAnywhere, Category = "Inventory", meta=(InlineEditConditionToggle))
	uint8 bShouldEquip:1 = false;

};

USTRUCT()
struct FStorageSetupDataProxy
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category="Inventory", meta=(ExcludeBaseStruct))
	TInstancedStruct<FInventoryStorageSetupData> SetupData;

	const FInventoryStorageSetupData* GetData() const;
	TSubclassOf<UInventoryStorage> GetStorageClass() const;

};

USTRUCT()
struct FInventoryEquipmentSlotProxy
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category="Inventory")
	EInventoryEquipmentSlot SlotId = EInventoryEquipmentSlot::Invalid;

	// What type of equipment can be put into this slot
	// (e.g. "Equipment.Weapons" allows ant weapons, and "Equipment.Weapon.Sword" allows any sword but not axes) 
	UPROPERTY(EditAnywhere, Category="Inventory", meta=(Categories="GameItems.Equipment"))
	FGameplayTag EquipmentTypeTag;
};

/**
 * 
 */
UCLASS()
class INVENTORY_API UInventorySetupData : public UDataAsset
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditAnywhere, Category="Inventory")
	FStorageSetupDataProxy InventoryStorage;

	UPROPERTY(EditAnywhere, Category="Inventory")
	TArray<FInventoryEquipmentSlotProxy> EquipmentSlots;

	// Default startup inventory (including equipment)
	UPROPERTY(EditAnywhere, Category = "Inventory")
	TArray<FInventoryItemProxy> DefaultStartupInventory;

	// Startup inventory (including equipment) for characters (by gameplay tag)
	UPROPERTY(EditAnywhere, Category = "Inventory", meta=(ForceInlineRow))
	TMap<FGameplayTag, FInventoryItemProxy> StartupInventoryForCharacters;
};
