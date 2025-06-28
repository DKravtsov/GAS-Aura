// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryCharacterWidget.generated.h"

/**
 * 
 */
UCLASS(MinimalAPI, Abstract)
class UInventoryCharacterWidget : public UUserWidget
{
	GENERATED_BODY()

	TWeakObjectPtr<USkeletalMeshComponent> Mesh;
	bool bIsDragging = false;

	FVector2D MousePosition;
	
public:

	//~ Begin of UUserWidget interface
	INVENTORY_API virtual void NativeOnInitialized() override;
	INVENTORY_API virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	INVENTORY_API virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	INVENTORY_API virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	INVENTORY_API virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	//~ End of UUserWidget interface
};
