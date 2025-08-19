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

	UPROPERTY(EditAnywhere, Category = "Inventory", meta=(EditCondition="bOverridePickupMessage"))
	FText PickupMessage;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSoftObjectPtr<UInventoryItemData> ItemData;

	UPROPERTY(EditInstanceOnly, Category="Inventory", meta=(EditCondition="bOverrideItemManifest"), Replicated)
	FInventoryItemManifest ItemManifest;

	// Override stack count, not replicated, will override the value in the manifest
	UPROPERTY(EditInstanceOnly, Category = "Inventory")
	int32 StackCount = -1;

	UPROPERTY(EditInstanceOnly, Category = "Inventory", meta=(InlineEditConditionToggle))
	bool bOverrideItemManifest = false;

	UPROPERTY(EditInstanceOnly, Category = "Inventory", meta=(InlineEditConditionToggle))
	bool bOverridePickupMessage = false;

	bool bCachedStackable;
	
public:

	INVENTORY_API UInventoryItemComponent();

	FText GetPickupMessage() const;

	const FInventoryItemManifest& GetItemManifest() const { return ItemManifest; }
	FInventoryItemManifest& GetItemManifestMutable() { return ItemManifest; }

	void PickedUp();

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	INVENTORY_API void InitItemManifestFrom(const FInventoryItemManifest& ItemManifestToCopy);

#if WITH_EDITOR
	UFUNCTION(CallInEditor, Category = "Inventory")
	void CopyManifestFromData();
#endif

	const FText& GetItemDisplayName() const; 
	int32 GetStackSize() const;
	
protected:

	INVENTORY_API virtual void BeginPlay() override;

	UFUNCTION(BlueprintImplementableEvent, Category="Inventory", meta=(DisplayName="On Picked Up"))
	void BP_PickedUp();

private:

	FText GetFormattedMessage(const FText& Fmt) const;;

};
