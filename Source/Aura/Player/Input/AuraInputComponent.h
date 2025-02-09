// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "AbilitySystem/Data/Input/AuraInputConfig.h"
#include "AuraInputComponent.generated.h"

/**
 *
 */
UCLASS()
class AURA_API UAuraInputComponent : public UEnhancedInputComponent
{
    GENERATED_BODY()

public:

    template<class TUserObject, typename TCallbackFunc>
    void BindNativeInputAction(class UAuraInputConfig* InputConfig, const FGameplayTag& InputTag, ETriggerEvent TriggerEvent, TUserObject* ContextObject, TCallbackFunc Func)
    {
        checkf(InputConfig, TEXT("Input config data asset is null, cannot proceed with binding."));

        if (UInputAction* FoundAction = InputConfig->FindInputActionByTag(InputTag))
        {
            BindAction(FoundAction, TriggerEvent, ContextObject, Func, InputTag);
        }
    }

    template<class TUserObject, typename TCallbackPressFunc, typename TCallbackReleaseFunc, typename TCallbackHeldFunc>
    void BindAbilityInputAction(class UAuraInputConfig* InputConfig, TUserObject* ContextObject, TCallbackPressFunc InputPressFunc, TCallbackReleaseFunc InputReleaseFunc, TCallbackHeldFunc InputHeldFunc = nullptr)
    {
        checkf(InputConfig, TEXT("Input config data asset is null, cannot proceed with binding."));

        for (const FAuraInputAction& Config : InputConfig->AbilityInputActions)
        {
            if (!Config.IsValid())
                continue;
            BindAction(Config.InputAction, ETriggerEvent::Started, ContextObject, InputPressFunc, Config.InputTag);
            BindAction(Config.InputAction, ETriggerEvent::Completed, ContextObject, InputReleaseFunc, Config.InputTag);
            if (InputHeldFunc)
                BindAction(Config.InputAction, ETriggerEvent::Triggered, ContextObject, InputHeldFunc, Config.InputTag);
        }
    }
};
