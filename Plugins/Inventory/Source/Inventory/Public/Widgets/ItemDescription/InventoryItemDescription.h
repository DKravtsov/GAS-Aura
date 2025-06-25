// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryItemDescription.generated.h"

/**
 * 
 */
UCLASS(MinimalAPI, Abstract)
class UInventoryItemDescription : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class USizeBox> SizeBox_Root;

public:

	FVector2D GetBoxSize() const;
};
