// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractableInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
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

    virtual void HighlightActor() = 0;
    virtual void UnhighlightActor() = 0;
};
