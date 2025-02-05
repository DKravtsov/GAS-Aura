// Copyright 4sandwiches


#include "UI/HUD/AuraHUD.h"
#include "UI/Widgets/AuraUserWidget.h"

AAuraHUD::AAuraHUD()
{
}

void AAuraHUD::BeginPlay()
{
    Super::BeginPlay();

    if (OverlayWidgetClass == nullptr)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("OverlayWidgetClass is not set"));
        return;
    }

    auto NewWidget = CreateWidget<UAuraUserWidget>(GetWorld(), OverlayWidgetClass);
    NewWidget->AddToViewport();
}
