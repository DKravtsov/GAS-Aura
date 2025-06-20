// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryInfoMessage.generated.h"

/**
 * 
 */
UCLASS()
class UInventoryInfoMessage : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> Text_Message;

	UPROPERTY(EditAnywhere, Category="Inventory")
	float MessageLifeTime = 3.f;

	FTimerHandle MessageTimer;
	bool bMessageActive = false;

public:

	UFUNCTION(BlueprintImplementableEvent, Category="Inventory")
	INVENTORY_API void ShowMessage();

	UFUNCTION(BlueprintImplementableEvent, Category="Inventory")
	INVENTORY_API void HideMessage();

	INVENTORY_API void SetMessage(const FText& Message);

	virtual void NativeOnInitialized() override;

};
