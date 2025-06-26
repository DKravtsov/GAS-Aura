// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "InventoryCompositeBase.h"
#include "InventoryLeaf.generated.h"

/**
 * 
 */
UCLASS(MinimalAPI, Abstract)
class UInventoryLeaf : public UInventoryCompositeBase
{
	GENERATED_BODY()

public:
	//~ Begin of UInventoryCompositeBase interface
	virtual void ApplyFunction(FFuncType Func) override;
	//~ End of UInventoryCompositeBase interface


	virtual void ApplyWorkFunction() override;

	UFUNCTION(BlueprintCallable)
	void Test_Function_Leaf(UInventoryCompositeBase* Composite);
};
