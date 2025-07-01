// Copyright 4sandwiches


#include "Items/Components/InventoryHighlightableSkeletalMesh.h"

void UInventoryHighlightableSkeletalMesh::Highlight_Implementation()
{
	SetOverlayMaterial(HighlightMaterial);
}

void UInventoryHighlightableSkeletalMesh::UnHighlight_Implementation()
{
	SetOverlayMaterial(nullptr);
}
