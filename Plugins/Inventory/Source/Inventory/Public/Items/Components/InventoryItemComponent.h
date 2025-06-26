// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Items/Manifest/InventoryItemManifest.h"
#include "InventoryItemComponent.generated.h"


class UInventoryItemData;

UCLASS(MinimalAPI, ClassGroup=(Inventory), Blueprintable, meta=(BlueprintSpawnableComponent))
class UInventoryItemComponent : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Inventory")
	FText PickupMessage;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSoftObjectPtr<UInventoryItemData> ItemData;

	UPROPERTY(EditInstanceOnly, Category="Inventory", meta=(EditCondition="bOverrideItemManifest"))
	FInventoryItemManifest ItemManifest;

	UPROPERTY(EditInstanceOnly, Category = "Inventory", meta=(InlineEditConditionToggle))
	bool bOverrideItemManifest = false;
	
public:

	INVENTORY_API UInventoryItemComponent();

	FText GetPickupMessage() { return PickupMessage; }

	FInventoryItemManifest GetItemManifest() const { return ItemManifest; }

	void PickedUp();

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	INVENTORY_API void InitItemManifestFrom(const FInventoryItemManifest& ItemManifestToCopy);

protected:

	INVENTORY_API virtual void BeginPlay() override;

	UFUNCTION(BlueprintImplementableEvent, Category="Inventory", meta=(DisplayName="On Picked Up"))
	void BP_PickedUp();
};
