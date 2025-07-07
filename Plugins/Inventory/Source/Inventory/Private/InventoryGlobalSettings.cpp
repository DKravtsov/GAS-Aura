// Copyright 4sandwiches


#include "InventoryGlobalSettings.h"
#include "InventoryManagement/Storage/SpatialStorage/InventorySpatialStorageGrid.h"

UInventoryGlobalSettings::UInventoryGlobalSettings(const FObjectInitializer& ObjectInitializer)
{
	SpatialStorageGridClass = UInventorySpatialStorageGrid::StaticClass();
}

const UInventoryGlobalSettings& UInventoryGlobalSettings::Get()
{
	const UInventoryGlobalSettings* GlobalSettings = GetDefault<UInventoryGlobalSettings>();
	return *GlobalSettings;
}

TSubclassOf<UInventorySpatialStorageGrid> UInventoryGlobalSettings::GetSpatialStorageGridClass()
{
	return Get().SpatialStorageGridClass.LoadSynchronous();
}
