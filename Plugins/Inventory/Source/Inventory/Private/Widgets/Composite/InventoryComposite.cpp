// Copyright 4sandwiches


#include "Widgets/Composite/InventoryComposite.h"

#include "Blueprint/WidgetTree.h"

void UInventoryComposite::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	WidgetTree->ForEachWidget([this](UWidget* Widget)
	{
		if (UInventoryCompositeBase* Composite = Cast<UInventoryCompositeBase>(Widget); IsValid(Composite))
		{
			Children.Emplace(Composite);
			Composite->Collapse();
		}
	});
}

void UInventoryComposite::Collapse()
{
	for (UInventoryCompositeBase* Child : Children)
	{
		Child->Collapse();
	}
}

void UInventoryComposite::ApplyFunction(FFuncType Func)
{
	for (UInventoryCompositeBase* Child : Children)
	{
		Child->ApplyFunction(Func);
	}
}

void UInventoryComposite::Test_Function_Three(UInventoryCompositeBase* Composite)
{
	UE_LOG(LogTemp, Warning, TEXT("Test_Function_Three()"));
}

void UInventoryComposite::ApplyWorkFunction()
{
	for (UInventoryCompositeBase* Child : Children)
	{
		Child->ApplyWorkFunction();
	}
}
