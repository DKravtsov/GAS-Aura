// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractableInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, Blueprintable)
class UInteractableInterface : public UInterface
{
    GENERATED_BODY()
};

/**
 *
 */
class AURA_API IInteractableInterface
{
    GENERATED_BODY()

public:

    UFUNCTION(BlueprintNativeEvent)
    void HighlightActor();

    UFUNCTION(BlueprintNativeEvent)
    void UnhighlightActor();
};
