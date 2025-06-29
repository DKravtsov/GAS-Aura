// Copyright 4sandwiches


#include "Items/Fragments/InventoryItemFragment.h"

#include "NativeGameplayTags.h"
#include "EquipmentManagement/EquipActor/InventoryEquipActor.h"
#include "Widgets/Composite/InventoryCompositeBase.h"
#include "Widgets/Composite/InventoryLeafImage.h"
#include "Widgets/Composite/InventoryLeafLabeledValue.h"
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
	FInventoryItemPropertyFragment::Assimilate(Composite);

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
	FInventoryItemPropertyFragment::Assimilate(Composite);
	if (MatchesWidgetTag(Composite))
	{
		if (const auto LeafText = Cast<UInventoryLeafText>(Composite))
		{
			LeafText->SetText(FragmentText);
		}
	}
}

void FInventoryNumericValue::Initialize()
{
	if (!bInitialized)
	{
		Value = FMath::FRandRange(MinMaxValue.X, MinMaxValue.Y);
		bInitialized = true;
	}
}

FInventoryItemLabeledValueFragment::FInventoryItemLabeledValueFragment()
{
	NumericValue = TInstancedStruct<FInventoryNumericValue>::Make();
}

void FInventoryItemLabeledValueFragment::Assimilate(UInventoryCompositeBase* Composite) const
{
	FInventoryItemPropertyFragment::Assimilate(Composite);
	FInventoryItemPropertyFragment::Assimilate(Composite);
	if (MatchesWidgetTag(Composite))
	{
		if (const auto LabeledText = Cast<UInventoryLeafLabeledValue>(Composite))
		{
			LabeledText->SetLabelText(LabelText, !bEnableLabel);
			FNumberFormattingOptions Options;
            Options.MinimumFractionalDigits = MinMaxFractionalDigits.X;
            Options.MaximumFractionalDigits = MinMaxFractionalDigits.Y;
			Options.UseGrouping = true;
			LabeledText->SetValueText(FText::AsNumber(GetValue(), &Options), !bEnableValue);
		}
	}
}

void FInventoryItemLabeledValueFragment::Manifest()
{
	FInventoryItemPropertyFragment::Manifest();
	if (auto* Struct = NumericValue.GetMutablePtr())
	{
		Struct->Initialize();
	}
	else
	{
		bEnableValue = false;
	}
}

FInventoryItemStackableFragment::FInventoryItemStackableFragment()
{
	SetFragmentTag(InventoryFragmentTags::FragmentTag_Stackable);
}

void FInventoryItemPropertyFragment::Assimilate(UInventoryCompositeBase* Composite) const
{
	if (MatchesWidgetTag(Composite))
	{
		Composite->Expand();
	}
}

bool FInventoryItemPropertyFragment::MatchesWidgetTag(const UInventoryCompositeBase* Composite) const
{
	return GetFragmentTag().MatchesTagExact(Composite->GetFragmentTag());
}

void FInventoryItemConsumableFragment::OnConsume(const APlayerController* PC) const
{
	for (const auto& Modifier : ConsumeModifiers)
	{
		Modifier.Get().OnConsume(PC);
	}
}

void FInventoryItemConsumableFragment::Assimilate(UInventoryCompositeBase* Composite) const
{
	FInventoryItemPropertyFragment::Assimilate(Composite);
	for (const auto& Modifier : ConsumeModifiers)
	{
		Modifier.Get().Assimilate(Composite);
	}
}

void FInventoryItemConsumableFragment::Manifest()
{
	FInventoryItemPropertyFragment::Manifest();
	for (auto& Modifier : ConsumeModifiers)
	{
		Modifier.GetMutable().Manifest();
	}
}

void FInventoryItemEquipmentFragment::SetEquippedActor(AInventoryEquipActor* EquipActor)
{
	EquippedActor = EquipActor;
}

void FInventoryItemEquipmentFragment::OnEquip(const APlayerController* PC)
{
	if (bEquipped)
		return;
	bEquipped = true;
	for (auto& Modifier : EquipModifiers)
	{
		Modifier.Get().OnEquip(PC);
	}
}

void FInventoryItemEquipmentFragment::OnUnequip(const APlayerController* PC)
{
	if (!bEquipped)
		return;
	bEquipped = false;
	for (auto& Modifier : EquipModifiers)
	{
		Modifier.Get().OnUnequip(PC);
	}
}

void FInventoryItemEquipmentFragment::Assimilate(UInventoryCompositeBase* Composite) const
{
	FInventoryItemPropertyFragment::Assimilate(Composite);
	for (const auto& Modifier : EquipModifiers)
	{
		Modifier.Get().Assimilate(Composite);
	}
}

void FInventoryItemEquipmentFragment::Manifest()
{
	FInventoryItemPropertyFragment::Manifest();
	for (auto& Modifier : EquipModifiers)
	{
		Modifier.GetMutable().Manifest();
	}
}

AInventoryEquipActor* FInventoryItemEquipmentFragment::SpawnEquippedActor(USkeletalMeshComponent* ParentMesh) const
{
	if (!IsValid(ParentMesh) || !IsValid(EquippedActorClass.LoadSynchronous()))
		return nullptr;

	UWorld* World = ParentMesh->GetWorld();
	if (!IsValid(World))
		return nullptr;
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.Instigator = Cast<APawn>(ParentMesh->GetOwner());
	AInventoryEquipActor* SpawnedActor = World->SpawnActor<AInventoryEquipActor>(EquippedActorClass.Get(), SpawnParams);
	if (IsValid(SpawnedActor))
	{
		SpawnedActor->AttachToComponent(ParentMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, SocketAttachPoint);
	}
	return SpawnedActor;
}

void FInventoryItemEquipmentFragment::DestroyEquipActor() const
{
	if (EquippedActor.IsValid())
	{
		EquippedActor->Destroy();
	}
}

void FInventoryExampleEquipModifier::OnEquip(const APlayerController* PC) const
{
	
	GEngine->AddOnScreenDebugMessage(
		-1,
		5.f,
		FColor::Green,
		FString::Printf(TEXT("Item Equipped. Strength increased by: %s"),
		*FString::SanitizeFloat(GetValue())));
}

void FInventoryExampleEquipModifier::OnUnequip(const APlayerController* PC) const
{
	GEngine->AddOnScreenDebugMessage(
		-1,
		5.f,
		FColor::Yellow,
		FString::Printf(TEXT("Item unequipped. Strength decreased by: %s"),
		*FString::SanitizeFloat(GetValue())));
}
