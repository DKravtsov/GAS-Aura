// Copyright 4sandwiches


#include "Widgets/CharacterDisplay/InventoryCharacterWidget.h"

#include "Blueprint/WidgetLayoutLibrary.h"
#include "EquipmentManagement/ProxyMesh/InventoryProxyMeshActor.h"
#include "Kismet/GameplayStatics.h"

void UInventoryCharacterWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(GetOwningPlayer(), AInventoryProxyMeshActor::StaticClass(), Actors);
	if (Actors.IsEmpty())
		return;

	if (AInventoryProxyMeshActor* ProxyMesh = Cast<AInventoryProxyMeshActor>(Actors[0]); IsValid(ProxyMesh))
	{
		Mesh = ProxyMesh->GetMesh();
	}
}

void UInventoryCharacterWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (bIsDragging && Mesh.IsValid())
	{
		auto LastMousePos = MousePosition;
		MousePosition = UWidgetLayoutLibrary::GetMousePositionOnViewport(GetOwningPlayer());

		const float HorDelta = MousePosition.X - LastMousePos.X;
		Mesh->AddRelativeRotation(FRotator(0.f, -HorDelta, 0.f));
	}
}

FReply UInventoryCharacterWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		MousePosition = UWidgetLayoutLibrary::GetMousePositionOnViewport(GetOwningPlayer());
		bIsDragging = true;
		return FReply::Handled();
	}
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

FReply UInventoryCharacterWidget::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		bIsDragging = false;
		return FReply::Handled();
	}
	return Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);
}

void UInventoryCharacterWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
	bIsDragging = false;
}
