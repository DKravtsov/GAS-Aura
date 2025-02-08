// Copyright 4sandwiches


#include "UI/HUD/AuraHUD.h"
#include "UI/Widgets/AuraUserWidget.h"
#include "UI/WidgetController/OverlayWidgetController.h"
#include "UI/WidgetController/AttributeMenuWidgetController.h"

AAuraHUD::AAuraHUD()
{
}

UOverlayWidgetController* AAuraHUD::GetOverlayWidgetController()
{
    if (OverlayWidgetController == nullptr)
    {
        check(OverlayWidgetControllerClass != nullptr);
        auto WidgetController = NewObject<UOverlayWidgetController>(this, OverlayWidgetControllerClass);
        WidgetController->InitWidgetController(GetOwningPlayerController());
        WidgetController->BindCallbacks();
        OverlayWidgetController = WidgetController;
    }
    return OverlayWidgetController;
}

UAttributeMenuWidgetController* AAuraHUD::GetAttributeMenuWidgetController()
{
    if (AttributeMenuWidgetController == nullptr)
    {
        check(AttributeMenuWidgetControllerClass != nullptr);
        auto WidgetController = NewObject<UAttributeMenuWidgetController>(this, AttributeMenuWidgetControllerClass);
        WidgetController->InitWidgetController(GetOwningPlayerController());
        WidgetController->BindCallbacks();
        AttributeMenuWidgetController = WidgetController;
    }
    return AttributeMenuWidgetController;
}

void AAuraHUD::InitOverlay()
{
    if (OverlayWidgetClass == nullptr)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("AuraHUD: OverlayWidgetClass is not set"));
        return;
    }
    if (OverlayWidgetControllerClass == nullptr)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("AuraHUD: OverlayWidgetControllerClass is not set"));
        return;
    }

    auto WidgetController = GetOverlayWidgetController();

    OverlayWidget = CreateWidget<UAuraUserWidget>(GetWorld(), OverlayWidgetClass);
    OverlayWidget->SetWidgetController(WidgetController);
    WidgetController->BroadcastInitialValues();
    OverlayWidget->AddToViewport();
}

