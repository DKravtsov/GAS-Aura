// Copyright 4sandwiches


#include "UI/WidgetController/AuraWidgetController.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/Pawn.h"

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

    if (auto Interface = Cast<IAbilitySystemInterface>(PlayerState))
    {
        AbilitySystemComponent = Interface->GetAbilitySystemComponent();
    }
    else if (APawn* ControlledPawn = PC->GetPawn())
    {
        Interface = Cast<IAbilitySystemInterface>(ControlledPawn);
        if (Interface != nullptr)
        {
            AbilitySystemComponent = Interface->GetAbilitySystemComponent();
        }
    }
    if (IsValid(AbilitySystemComponent))
    {
        AttributeSet = const_cast<UAttributeSet*>(AbilitySystemComponent->GetAttributeSet(UAttributeSet::StaticClass()));
    }
}
