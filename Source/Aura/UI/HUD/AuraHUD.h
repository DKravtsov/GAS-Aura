// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "AuraHUD.generated.h"

class UOverlayWidgetController;

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

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UOverlayWidgetController> OverlayWidgetControllerClass;

    mutable TObjectPtr<UOverlayWidgetController> OverlayWidgetController;

public:

    AAuraHUD();

    UOverlayWidgetController* GetOverlayWidgetController();

    void InitOverlay();
};
