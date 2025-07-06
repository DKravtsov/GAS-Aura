// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "InventorySlottedItemWidget.h"
#include "InventoryEquippedSlottedItemWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEquippedSlottedItemClickedSignature, UInventoryEquippedSlottedItemWidget*, SlottedItem);

/**
 * 
 */
UCLASS()
class UInventoryEquippedSlottedItemWidget : public UInventorySlottedItemWidget
{
	GENERATED_BODY()
public:

	FEquippedSlottedItemClickedSignature OnEquippedSlottedItemClicked;

private:
	UPROPERTY()
	FGameplayTag EquipmentTypeTag;

public:

	const FGameplayTag& GetEquipmentTypeTag() const {return EquipmentTypeTag;}
	void SetEquipmentTypeTag(const FGameplayTag& NewTag) { EquipmentTypeTag = NewTag; }

	//~ Being of UUserWidget interface
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	//~ End of UUserWidget interface

};
