// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryGridSlot.generated.h"

class UImage;

UCLASS()
class UInventoryGridSlot : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_GridSlot;

	int32 TileIndex;
	
public:

	int32 GetTileIndex() const { return TileIndex; }
	void SetTileIndex(const int32 NewTileIndex) { TileIndex = NewTileIndex; }

private:

	
};
