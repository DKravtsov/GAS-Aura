// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "InventoryGridTypes.h"
#include "InventoryManagement/FastArray/InventoryFastArray.h"
#include "InventoryManagementComponentBase.generated.h"


class UInventoryStorage;

UCLASS(MinimalAPI, ClassGroup=(Inventory), Blueprintable, Abstract, meta=(BlueprintSpawnableComponent))
class UInventoryManagementComponentBase : public UActorComponent
{
	GENERATED_BODY()

public:
	FInventoryItemChangedDelegate OnItemAdded;
	FInventoryItemChangedDelegate OnItemRemoved;
	FInventoryItemGridChangedDelegate OnStackChanged;

protected:

	// Inventory Setup Data
	UPROPERTY(EditDefaultsOnly, Category="Inventory")
	TSoftObjectPtr<class UInventorySetupData> InventorySetupData;

	// Responsible for HOW the inventory is stored
	UPROPERTY(Replicated)
	TObjectPtr<UInventoryStorage> InventoryStorage;

	// List of WHAT is stored in the inventory
	UPROPERTY(Replicated)
	FInventoryFastArray InventoryList;


public:
	UInventoryManagementComponentBase();
	
	UInventoryStorage* GetInventoryStorage() const { return InventoryStorage; }

	void AddRepSubObj(UObject* SubObj);

	//#if UE_WITH_CHEAT_MANAGER
	INVENTORY_API virtual void DebugPrintStorage() const {}
	//#endif//UE_WITH_CHEAT_MANAGER

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:

	virtual void BeginPlay() override;

	virtual void CreateInventoryStorage();

};
