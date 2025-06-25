// Copyright 4sandwiches


#include "Items/Fragments/InventoryItemFragment.h"

#include "NativeGameplayTags.h"

namespace InventoryFragmentTags
{
	UE_DEFINE_GAMEPLAY_TAG(FragmentTag_Grid, "FragmentTag.Grid");
	UE_DEFINE_GAMEPLAY_TAG(FragmentTag_Image, "FragmentTag.Image");
	UE_DEFINE_GAMEPLAY_TAG(FragmentTag_Stackable, "FragmentTag.Stackable");
	UE_DEFINE_GAMEPLAY_TAG(FragmentTag_Consumable, "FragmentTag.Consumable");
}

FInventoryItemGridFragment::FInventoryItemGridFragment()
{
	SetFragmentTag(InventoryFragmentTags::FragmentTag_Grid);
}

FInventoryItemImageFragment::FInventoryItemImageFragment()
{
	SetFragmentTag(InventoryFragmentTags::FragmentTag_Image);
}

FInventoryItemStackableFragment::FInventoryItemStackableFragment()
{
	SetFragmentTag(InventoryFragmentTags::FragmentTag_Stackable);
}
