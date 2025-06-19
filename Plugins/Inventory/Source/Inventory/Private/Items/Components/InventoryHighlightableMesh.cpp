// Copyright 4sandwiches


#include "Items/Components/InventoryHighlightableMesh.h"

void UInventoryHighlightableMesh::Highlight_Implementation()
{
	SetOverlayMaterial(HighlightMaterial);
}

void UInventoryHighlightableMesh::UnHighlight_Implementation()
{
	SetOverlayMaterial(nullptr);
}
