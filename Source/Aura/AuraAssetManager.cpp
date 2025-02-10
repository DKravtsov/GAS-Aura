// Copyright 4sandwiches


#include "AuraAssetManager.h"
#include "AbilitySystemGlobals.h"

UAuraAssetManager& UAuraAssetManager::Get()
{
    check(GEngine);
    return *Cast<UAuraAssetManager>(GEngine->AssetManager);
}

void UAuraAssetManager::StartInitialLoading()
{
    Super::StartInitialLoading();

    // This is required to use TargetData
    UAbilitySystemGlobals::Get().InitGlobalData();
}
