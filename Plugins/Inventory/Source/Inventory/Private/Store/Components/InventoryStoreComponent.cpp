// Copyright 4sandwiches


#include "Store/Components/InventoryStoreComponent.h"

#include "Components/SlateWrapperTypes.h"
#include "Net/UnrealNetwork.h"
#include "Widgets/Inventory/Base/InventoryWidgetBase.h"

#include "DebugHelper.h"


// Sets default values for this component's properties
UInventoryStoreComponent::UInventoryStoreComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);

}

void UInventoryStoreComponent::ToggleInventoryMenu()
{
	if (bInventoryMenuOpen)
	{
		CloseInventoryMenu();
	}
	else
	{
		OpenInventoryMenu();
	}
}

void UInventoryStoreComponent::BeginPlay()
{
	LOG_NETFUNCTIONCALL_COMPONENT
	
	Super::BeginPlay();

}

void UInventoryStoreComponent::ConstructInventory()
{
	LOG_NETFUNCTIONCALL_COMPONENT
	
	checkf(!InventoryMenuClass.IsNull(), TEXT("Forgot to set InventoryMenuClass in [%s]"), *GetNameSafe(GetClass()));
	
	const TSubclassOf<UInventoryWidgetBase> LoadedInventoryMenuClass = InventoryMenuClass.LoadSynchronous();
	check(LoadedInventoryMenuClass);

	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	check(PC);
	InventoryMenu = CreateWidget<UInventoryWidgetBase>(PC, LoadedInventoryMenuClass);
	InventoryMenu->SetContextObject(this);
	InventoryMenu->AddToViewport();
}

void UInventoryStoreComponent::OpenInventoryMenu()
{
	if (!IsValid(InventoryMenu))
	{
		ConstructInventory();
	}
	InventoryMenu->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	InventoryMenu->OnOpenedMenu();
	bInventoryMenuOpen = true;
}

void UInventoryStoreComponent::CloseInventoryMenu()
{
	if (!IsValid(InventoryMenu))
		return;
	InventoryMenu->SetVisibility(ESlateVisibility::Collapsed);
	InventoryMenu->OnCloseMenu();
	bInventoryMenuOpen = false;
}

void UInventoryStoreComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UInventoryStoreComponent, InventoryList);
}
