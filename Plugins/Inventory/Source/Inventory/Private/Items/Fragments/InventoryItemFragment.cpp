// Copyright 4sandwiches


#include "Items/Fragments/InventoryItemFragment.h"

#include "NativeGameplayTags.h"

namespace InventoryFragmentTags
{
	UE_DEFINE_GAMEPLAY_TAG(FragmentTag_Grid, "FragmentTag.Grid");
	UE_DEFINE_GAMEPLAY_TAG(FragmentTag_Image, "FragmentTag.Image");
}

FInventoryItemGridFragment::FInventoryItemGridFragment()
{
	SetFragmentTag(InventoryFragmentTags::FragmentTag_Grid);
}

FInventoryItemImageFragment::FInventoryItemImageFragment()
{
	SetFragmentTag(InventoryFragmentTags::FragmentTag_Image);
}
