// Copyright 4sandwiches


#include "AuraAssetManager.h"

UAuraAssetManager& UAuraAssetManager::Get()
{
    check(GEngine);
    return *Cast<UAuraAssetManager>(GEngine->AssetManager);
}

void UAuraAssetManager::StartInitialLoading()
{
    Super::StartInitialLoading();


}
