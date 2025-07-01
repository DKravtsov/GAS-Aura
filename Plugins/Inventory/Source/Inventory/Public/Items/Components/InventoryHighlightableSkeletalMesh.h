// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"
#include "Interfaces/InventoryHighlightableInterface.h"
#include "InventoryHighlightableSkeletalMesh.generated.h"

/**
 * 
 */
UCLASS(MinimalAPI)
class UInventoryHighlightableSkeletalMesh : public USkeletalMeshComponent, public IInventoryHighlightableInterface
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category="Inventory")
	TObjectPtr<UMaterialInterface> HighlightMaterial;

public:

	//~ Begin of IInventoryHighlightableInterface interface
	INVENTORY_API virtual void Highlight_Implementation() override;
	INVENTORY_API virtual void UnHighlight_Implementation() override;
	//~ End of IInventoryHighlightableInterface interface
};
