// Copyright 4sandwiches


#include "InventoryGlobalSettings.h"
#include "InventoryManagement/Storage/SpatialStorage/InventoryStorageGrid.h"

UInventoryGlobalSettings::UInventoryGlobalSettings(const FObjectInitializer& ObjectInitializer)
{
	SpatialStorageGridClass = UInventoryStorageGrid::StaticClass();
}

const UInventoryGlobalSettings& UInventoryGlobalSettings::Get()
{
	const UInventoryGlobalSettings* GlobalSettings = GetDefault<UInventoryGlobalSettings>();
	return *GlobalSettings;
}

TSubclassOf<UInventoryStorageGrid> UInventoryGlobalSettings::GetSpatialStorageGridClass()
{
	return Get().SpatialStorageGridClass.LoadSynchronous();
}
