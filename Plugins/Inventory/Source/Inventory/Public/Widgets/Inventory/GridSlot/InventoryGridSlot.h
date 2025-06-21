// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryGridSlot.generated.h"

class UImage;

enum class EInventoryGridSlotState : uint8
{
	Default,
	Occupied,
	Selected,
	GrayedOut,
};

UCLASS()
class UInventoryGridSlot : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_GridSlot;

	int32 TileIndex;

	EInventoryGridSlotState GridSlotState;

	UPROPERTY(EditAnywhere, Category="Inventory")
	FSlateBrush DefaultBrush;

	UPROPERTY(EditAnywhere, Category="Inventory")
	FSlateBrush OccupiedBrush;

	UPROPERTY(EditAnywhere, Category="Inventory")
	FSlateBrush SelectedBrush;

	UPROPERTY(EditAnywhere, Category="Inventory")
	FSlateBrush GrayedOutBrush;
	
public:

	int32 GetTileIndex() const { return TileIndex; }
	void SetTileIndex(const int32 NewTileIndex) { TileIndex = NewTileIndex; }

	EInventoryGridSlotState GetGridSlotState() const { return GridSlotState; }

	void SetDefaultTexture();
	void SetOccupiedTexture();
	void SetSelectedTexture();
	void SetGrayedOutTexture();
	
private:

	
};
