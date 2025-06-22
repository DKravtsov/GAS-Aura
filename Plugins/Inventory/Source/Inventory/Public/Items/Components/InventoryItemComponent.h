// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Items/Manifest/InventoryItemManifest.h"
#include "InventoryItemComponent.generated.h"


UCLASS(MinimalAPI, ClassGroup=(Inventory), Blueprintable, meta=(BlueprintSpawnableComponent))
class UInventoryItemComponent : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="Inventory")
	FInventoryItemManifest ItemManifest;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	FText PickupMessage;
	
public:

	INVENTORY_API UInventoryItemComponent();

	FText GetPickupMessage() { return PickupMessage; }

	FInventoryItemManifest GetItemManifest() const { return ItemManifest; }

	void PickedUp();

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

protected:

	UFUNCTION(BlueprintImplementableEvent, Category="Inventory", meta=(DisplayName="On Picked Up"))
	void BP_PickedUp();
};
