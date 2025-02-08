// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "AuraHUD.generated.h"

class UOverlayWidgetController;
class UAttributeMenuWidgetController;
class UAuraUserWidget;

/**
 *
 */
UCLASS()
class AURA_API AAuraHUD : public AHUD
{
    GENERATED_BODY()

    UPROPERTY()
    TObjectPtr<UAuraUserWidget> OverlayWidget;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UAuraUserWidget> OverlayWidgetClass;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UOverlayWidgetController> OverlayWidgetControllerClass;

    UPROPERTY()
    TObjectPtr<UOverlayWidgetController> OverlayWidgetController;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UAttributeMenuWidgetController> AttributeMenuWidgetControllerClass;

    UPROPERTY()
    TObjectPtr<UAttributeMenuWidgetController> AttributeMenuWidgetController;

public:

    AAuraHUD();

    // returns OverlayWidgetController, creates and initializes it if needed
    UOverlayWidgetController* GetOverlayWidgetController();

    // returns AttributeMenuWidgetController, creates and initializes it if needed
    UAttributeMenuWidgetController* GetAttributeMenuWidgetController();

    void InitOverlay();
};
