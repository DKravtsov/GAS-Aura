// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "AuraHUD.generated.h"

/**
 *
 */
UCLASS()
class AURA_API AAuraHUD : public AHUD
{
    GENERATED_BODY()

public:

    UPROPERTY()
    TObjectPtr<class UAuraUserWidget> OverlayWidget;

private:

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UAuraUserWidget> OverlayWidgetClass;

public:

    AAuraHUD();

protected:

    virtual void BeginPlay() override;
};
