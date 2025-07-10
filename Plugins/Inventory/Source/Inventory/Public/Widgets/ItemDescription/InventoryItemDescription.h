// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Composite/InventoryComposite.h"
#include "InventoryItemDescription.generated.h"

/**
 * 
 */
UCLASS(MinimalAPI, Abstract)
class UInventoryItemDescription : public UInventoryComposite
{
	GENERATED_BODY()

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class USizeBox> SizeBox_Root;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<class UNamedSlot> NamedSlot_EquippedDesc;

public:

	FVector2D GetBoxSize() const;

	virtual void Show();
	virtual void Hide();

	virtual void InsertEquippedDescription(UInventoryItemDescription* EquippedDescWidget);
};
