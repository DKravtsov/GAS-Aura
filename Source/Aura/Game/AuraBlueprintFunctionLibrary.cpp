// Copyright 4sandwiches


#include "Game/AuraBlueprintFunctionLibrary.h"

#include "AbilitySystem/AuraAbilitySystemTypes.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "UI/HUD/AuraHUD.h"
#include "Game/AuraGameModeBase.h"
#include "Characters/CombatInterface.h"

UOverlayWidgetController* UAuraBlueprintFunctionLibrary::GetOverlayWidgetController(const UObject* WorldContextObject)
{
    if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
    {
        checkf(PC->IsLocalController(), TEXT("The Player Controller must be a local player controller only."));

        if (AAuraHUD* AuraHUD = PC->GetHUD<AAuraHUD>())
        {
            return AuraHUD->GetOverlayWidgetController();
        }
    }

    return nullptr;
}

UAttributeMenuWidgetController* UAuraBlueprintFunctionLibrary::GetAttributeMenuWidgetController(const UObject* WorldContextObject)
{
    if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
    {
        checkf(PC->IsLocalController(), TEXT("The Player Controller must be a local player controller only."));

        if (AAuraHUD* AuraHUD = PC->GetHUD<AAuraHUD>())
        {
            return AuraHUD->GetAttributeMenuWidgetController();
        }
    }

    return nullptr;
}

UCharacterClassInfo* UAuraBlueprintFunctionLibrary::GetCharacterClassInfo(const UObject* WorldContextObject)
{
    if (auto GM = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject)))
    {
        return GM->CharacterClassInfo;
    }
    return nullptr;
}

int32 UAuraBlueprintFunctionLibrary::GetCharacterLevel(const AActor* Actor)
{
    if (const ICombatInterface* Interface = Cast<ICombatInterface>(Actor))
    {
        return Interface->GetCharacterLevel();
    }
    return 1;
}

bool UAuraBlueprintFunctionLibrary::IsBlockedHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
    // Note: we can cast to FAuraGameplayEffectContext because we set this in FAuraAbilitySystemGlobals which is set in DefaultGame.ini
    if (const FAuraGameplayEffectContext* EffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
    {
        return EffectContext->IsBlockedHit();
    }
    return false;
}

bool UAuraBlueprintFunctionLibrary::IsCriticalHit(const struct FGameplayEffectContextHandle& EffectContextHandle)
{
    // Note: we can cast to FAuraGameplayEffectContext because we set this in FAuraAbilitySystemGlobals which is set in DefaultGame.ini
    if (const FAuraGameplayEffectContext* EffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
    {
        return EffectContext->IsCriticalHit();
    }
    return false;
}

void UAuraBlueprintFunctionLibrary::SetIsBlockedHit(FGameplayEffectContextHandle& EffectContextHandle, bool bBlockedHit)
{
    // Note: we can cast to FAuraGameplayEffectContext because we set this in FAuraAbilitySystemGlobals which is set in DefaultGame.ini
    if (FAuraGameplayEffectContext* EffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
    {
        EffectContext->SetIsBlockedHit(bBlockedHit);
    }
}

void UAuraBlueprintFunctionLibrary::SetIsCriticalHit(FGameplayEffectContextHandle& EffectContextHandle, bool bCriticalHit)
{
    // Note: we can cast to FAuraGameplayEffectContext because we set this in FAuraAbilitySystemGlobals which is set in DefaultGame.ini
    if (FAuraGameplayEffectContext* EffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
    {
        EffectContext->SetIsCriticalHit(bCriticalHit);
    }
}
