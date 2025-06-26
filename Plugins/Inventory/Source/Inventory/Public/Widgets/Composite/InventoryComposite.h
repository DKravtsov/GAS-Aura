// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "InventoryCompositeBase.h"
#include "InventoryComposite.generated.h"

/**
 * 
 */
UCLASS(MinimalAPI, Abstract)
class UInventoryComposite : public UInventoryCompositeBase
{
	GENERATED_BODY()

	UPROPERTY(Transient)
	TArray<TObjectPtr<UInventoryCompositeBase>> Children;

public:

	//~ Begin of UUserWidget interface 
	virtual void NativeOnInitialized() override;
	//~ End of UUserWidget interface

	//~ Begin of UInventoryCompositeBase interface
	virtual void Collapse() override;
	virtual void ApplyFunction(FFuncType Func) override;
	//~ End of UInventoryCompositeBase interface

	UFUNCTION(BlueprintCallable)
	void Test_Function_Three(UInventoryCompositeBase* Composite);

	virtual void ApplyWorkFunction() override;
};
