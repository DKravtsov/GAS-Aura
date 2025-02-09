// Copyright 4sandwiches


#include "AbilitySystem/Data/Input/AuraInputConfig.h"
#include "InputAction.h"

const UInputAction* UAuraInputConfig::FindInputActionByTag(const FGameplayTag& InputTag, bool bLogNotFound) const
{
    if (const auto AuraInputAction = AbilityInputActions.FindByKey(InputTag))
    {
        return AuraInputAction->InputAction;
    }

    if (bLogNotFound)
    {
        UE_LOG(LogTemp, Error, TEXT("Cannot find info for the InputTag [%s] on the InputActionConfig [%s]"),
            *InputTag.ToString(), *GetNameSafe(this));
    }

    return nullptr;
}
