// Copyright 4sandwiches


#include "UI/WidgetController/AuraWidgetController.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/Pawn.h"
#include "AbilitySystemBlueprintLibrary.h"

UAuraWidgetController::UAuraWidgetController()
{
}

void UAuraWidgetController::InitWidgetController(APlayerController* PC)
{
    if (!IsValid(PC))
    {
        return;
    }

    PlayerController = PC;
    PlayerState = PC->GetPlayerState<APlayerState>();

    AbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(PlayerState);
    if (IsValid(AbilitySystemComponent))
    {
        AttributeSet = const_cast<UAttributeSet*>(AbilitySystemComponent->GetAttributeSet(UAttributeSet::StaticClass()));
    }
}

void UAuraWidgetController::BroadcastInitialValues()
{
}

void UAuraWidgetController::BindCallbacks()
{
}
