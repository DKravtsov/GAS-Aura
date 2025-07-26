// Copyright 4sandwiches


#include "InventoryManagement/Components/InventoryManagementComponentBase.h"
#include "InventoryManagement/Data/InventorySetupData.h"
#include "InventoryManagement/Storage/InventoryStorage.h"
#include "Net/UnrealNetwork.h"

#include "DebugHelper.h"


UInventoryManagementComponentBase::UInventoryManagementComponentBase()
	: InventoryList(this)
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
	bReplicateUsingRegisteredSubObjectList = true;
}

void UInventoryManagementComponentBase::AddRepSubObj(UObject* SubObj)
{
	if (IsUsingRegisteredSubObjectList() && IsReadyForReplication() && IsValid(SubObj))
	{
		AddReplicatedSubObject(SubObj);
	}
}

void UInventoryManagementComponentBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UInventoryManagementComponentBase, InventoryList);
	DOREPLIFETIME(UInventoryManagementComponentBase, InventoryStorage);
}

void UInventoryManagementComponentBase::BeginPlay()
{
	LOG_NETFUNCTIONCALL

	if (GetOwner()->HasAuthority())
	{
		CreateInventoryStorage();
	}

	Super::BeginPlay();
}

void UInventoryManagementComponentBase::CreateInventoryStorage()
{
	LOG_NETFUNCTIONCALL

	check(GetOwner()->HasAuthority());
	
	if (!ensure(!IsValid(InventoryStorage)))
		return;

	check(!InventorySetupData.IsNull());

	const FInventoryStorageSetupData* SetupData = InventorySetupData.LoadSynchronous()->InventoryStorage.GetData();
	check(SetupData != nullptr);

	InventoryStorage = NewObject<UInventoryStorage>(this, SetupData->StorageClass);	
	check(InventoryStorage);
	AddRepSubObj(InventoryStorage);

	InventoryStorage->SetupStorage(SetupData);
}
