// Copyright 4sandwiches


#include "Items/Fragments/InventoryItemFragment.h"

#include "NativeGameplayTags.h"
#include "Widgets/Composite/InventoryCompositeBase.h"
#include "Widgets/Composite/InventoryLeafImage.h"
#include "Widgets/Composite/InventoryLeafText.h"

namespace InventoryFragmentTags
{
	UE_DEFINE_GAMEPLAY_TAG(FragmentTag_Grid, "FragmentTag.Grid");
	UE_DEFINE_GAMEPLAY_TAG(FragmentTag_Image, "FragmentTag.Image");
	UE_DEFINE_GAMEPLAY_TAG(FragmentTag_Stackable, "FragmentTag.Stackable");
	UE_DEFINE_GAMEPLAY_TAG(FragmentTag_Consumable, "FragmentTag.Consumable");
	UE_DEFINE_GAMEPLAY_TAG(FragmentTag_ItemName, "FragmentTag.ItemName");
	UE_DEFINE_GAMEPLAY_TAG(FragmentTag_ItemDescription, "FragmentTag.ItemDescription");
}

FInventoryItemGridFragment::FInventoryItemGridFragment()
{
	SetFragmentTag(InventoryFragmentTags::FragmentTag_Grid);
}

FInventoryItemImageFragment::FInventoryItemImageFragment()
{
	SetFragmentTag(InventoryFragmentTags::FragmentTag_Image);
}

void FInventoryItemImageFragment::Assimilate(UInventoryCompositeBase* Composite) const
{
	FInventoryItemDescriptionFragment::Assimilate(Composite);

	if (MatchesWidgetTag(Composite))
	{
		if (const auto Image = Cast<UInventoryLeafImage>(Composite))
		{
			Image->SetImageFromSoftTexture(Icon);
			Image->SetBoxSize(IconDimensions);
			Image->SetImageSize(IconDimensions);
		}
	}
}

void FInventoryItemTextFragment::Assimilate(UInventoryCompositeBase* Composite) const
{
	FInventoryItemDescriptionFragment::Assimilate(Composite);
	if (MatchesWidgetTag(Composite))
	{
		if (const auto LeafText = Cast<UInventoryLeafText>(Composite))
		{
			LeafText->SetText(FragmentText);
		}
	}
}

FInventoryItemStackableFragment::FInventoryItemStackableFragment()
{
	SetFragmentTag(InventoryFragmentTags::FragmentTag_Stackable);
}

void FInventoryItemDescriptionFragment::Assimilate(UInventoryCompositeBase* Composite) const
{
	if (MatchesWidgetTag(Composite))
	{
		Composite->Expand();
	}
}

bool FInventoryItemDescriptionFragment::MatchesWidgetTag(UInventoryCompositeBase* Composite) const
{
	return GetFragmentTag().MatchesTagExact(Composite->GetFragmentTag());
}
