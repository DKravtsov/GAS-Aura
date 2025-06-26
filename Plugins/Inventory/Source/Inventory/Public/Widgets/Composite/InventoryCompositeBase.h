// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Blueprint/UserWidget.h"
#include "InventoryCompositeBase.generated.h"

/**
 * 
 */
UCLASS(MinimalAPI, Abstract)
class UInventoryCompositeBase : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Inventory", meta=(Categories="FragmentTag"))
	FGameplayTag FragmentTag;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	ESlateVisibility DefaultVisibility = ESlateVisibility::SelfHitTestInvisible;

	UPROPERTY(EditAnywhere, Category = "Inventory", meta=(FunctionReference, AllowFunctionLibraries, PrototypeFunction="/Script/Inventory.InventoryCompositeBase.Prototype_WorkFunction", DefaultBindingName="DoWork"))
	FMemberReference WorkFunction;

public:

	const FGameplayTag& GetFragmentTag() const { return FragmentTag; }
	void SetFragmentTag(const FGameplayTag& newFragmentTag) { FragmentTag = newFragmentTag; }

	virtual void Collapse();
	void Expand();

	using FFuncType = TFunction<void (UInventoryCompositeBase*)>;

	virtual void ApplyFunction(FFuncType Func) {}

#if WITH_EDITOR
	// This Prototype function defines the signature of the function for the editor
	UFUNCTION(BlueprintInternalUseOnly)
	void Prototype_WorkFunction(UInventoryCompositeBase* Composite) {}
#endif// WITH_EDITOR

	UFUNCTION(BlueprintCallable)
	void Test_Function_One(UInventoryCompositeBase* Composite);
	
	UFUNCTION(BlueprintCallable)
	void Test_Function_Two(UInventoryCompositeBase* Composite);


	virtual void ApplyWorkFunction() {}
	
protected:
	
	void CallWorkFunction(UInventoryCompositeBase* Composite);
};
