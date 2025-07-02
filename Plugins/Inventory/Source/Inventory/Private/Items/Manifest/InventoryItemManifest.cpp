// Copyright 4sandwiches


#include "Items/Manifest/InventoryItemManifest.h"

#include "Items/InventoryItem.h"
#include "Items/Components/InventoryItemComponent.h"
#include "Items/Fragments/InventoryItemFragment.h"
#include "Widgets/Composite/InventoryCompositeBase.h"


UInventoryItem* FInventoryItemManifest::Manifest(UObject* NewOuter)
{
	const auto NewItem = NewObject<UInventoryItem>(NewOuter, UInventoryItem::StaticClass());
	NewItem->SetItemManifest(*this);

	for (auto& Fragment : NewItem->GetItemManifestMutable().Fragments)
	{
		Fragment.GetMutable().Manifest();
	}
	ClearFragments();
	
	return NewItem;
}

UInventoryItem* FInventoryItemManifest::ManifestCopy(UObject* NewOuter) const
{
	const auto NewItem = NewObject<UInventoryItem>(NewOuter, UInventoryItem::StaticClass());
	NewItem->SetItemManifest(*this);

	for (auto& Fragment : NewItem->GetItemManifestMutable().Fragments)
	{
		Fragment.GetMutable().Manifest();
	}

	return NewItem;
}

AActor* FInventoryItemManifest::SpawnPickupActor(const UObject* WorldContextObject, const FVector& SpawnLocation, const FRotator& SpawnRotation) const
{
	if (!PickupActorClass || !IsValid(WorldContextObject) || !GEngine)
		return nullptr;

	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	if (AActor* SpawnedActor = World ? World->SpawnActor<AActor>(PickupActorClass, SpawnLocation, SpawnRotation, SpawnParams) : nullptr)
	{
		UInventoryItemComponent* ItemComponent = SpawnedActor->FindComponentByClass<UInventoryItemComponent>();
		check(ItemComponent != nullptr);
		ItemComponent->InitItemManifestFrom(*this);

		return SpawnedActor;
	}
	return nullptr;
}

void FInventoryItemManifest::AssimilateInventoryFragments(UInventoryCompositeBase* Composite) const
{
	const auto DescFragments = GetAllFragmentsOfType<FInventoryItemPropertyFragment>();
	for (const auto Fragment : DescFragments)
	{

		Composite->ApplyFunction([Fragment](UInventoryCompositeBase* Widget)
		{
			Fragment->Assimilate(Widget);
		});

		// test member reference
		Composite->ApplyWorkFunction();
	}
}

void FInventoryItemManifest::ClearFragments()
{
	for (auto& Fragment : Fragments)
	{
		Fragment.Reset();
	}
	Fragments.Empty();
}
