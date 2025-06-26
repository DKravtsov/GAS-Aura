// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "InventoryLeaf.h"
#include "InventoryLeafLabeledValue.generated.h"

/**
 * 
 */
UCLASS(MinimalAPI, Abstract)
class UInventoryLeafLabeledValue : public UInventoryLeaf
{
	GENERATED_BODY()

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UTextBlock> Text_Label;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UTextBlock> Text_Value;

	UPROPERTY(EditAnywhere, Category="Inventory")
	int32 LabelFontSize = 16;

	UPROPERTY(EditAnywhere, Category="Inventory")
	int32 ValueFontSize = 20;

public:

	//~ Begin of UUserWidget interface
	virtual void NativePreConstruct() override;
	//~ End of UUserWidget interface
	

	void SetLabelText(const FText& NewText, bool bCollapse = false);
	void SetValueText(const FText& NewText, bool bCollapse = false);
};
