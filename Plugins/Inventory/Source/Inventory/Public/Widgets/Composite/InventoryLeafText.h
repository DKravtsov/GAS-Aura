// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "InventoryLeaf.h"
#include "InventoryLeafText.generated.h"

/**
 * 
 */
UCLASS(MinimalAPI, Abstract)
class UInventoryLeafText : public UInventoryLeaf
{
	GENERATED_BODY()

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UTextBlock> Text_Body;

	UPROPERTY(EditAnywhere, Category="Inventory")
	int32 FontSize = 16;

public:

	//~ Begin of UUserWidget interface
	virtual void NativePreConstruct() override;
	//~ End of UUserWidget interface
	

	void SetText(const FText& NewText);
};
