// Copyright 4sandwiches


#include "UI/HUD/AuraHUD.h"
#include "UI/Widgets/AuraUserWidget.h"
#include "UI/WidgetController/OverlayWidgetController.h"
#include "UI/WidgetController/AttributeMenuWidgetController.h"
#include "UI/WidgetController/SpellMenuWidgetController.h"

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

USpellMenuWidgetController* AAuraHUD::GetSpellMenuWidgetController()
{
    if (SpellMenuWidgetController == nullptr)
    {
        check(SpellMenuWidgetControllerClass != nullptr);
        auto WidgetController = NewObject<USpellMenuWidgetController>(this, SpellMenuWidgetControllerClass);
        WidgetController->InitWidgetController(GetOwningPlayerController());
        WidgetController->BindCallbacks();
        SpellMenuWidgetController = WidgetController;
    }
    return SpellMenuWidgetController;
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

    OverlayWidget = CreateWidget<UAuraUserWidget>(GetWorld(), OverlayWidgetClass);
    OverlayWidget->AddToViewport();
    auto WidgetController = GetOverlayWidgetController();
    OverlayWidget->SetWidgetController(WidgetController);
    WidgetController->BroadcastInitialValues();
}

