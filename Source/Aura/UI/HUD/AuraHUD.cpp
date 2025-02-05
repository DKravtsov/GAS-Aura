// Copyright 4sandwiches


#include "UI/HUD/AuraHUD.h"
#include "UI/Widgets/AuraUserWidget.h"
#include "UI/WidgetController/OverlayWidgetController.h"

AAuraHUD::AAuraHUD()
{
}

UOverlayWidgetController* AAuraHUD::GetOverlayWidgetController()
{
    if (OverlayWidgetController == nullptr)
    {
        auto WidgetController = NewObject<UOverlayWidgetController>(this, OverlayWidgetControllerClass);
        WidgetController->InitWidgetController(GetOwningPlayerController());
        OverlayWidgetController = WidgetController;
    }
    return OverlayWidgetController;
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
    OverlayWidget->AddToViewport();
}

