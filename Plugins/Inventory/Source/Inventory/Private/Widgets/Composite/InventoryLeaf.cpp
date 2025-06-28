// Copyright 4sandwiches


#include "Widgets/Composite/InventoryLeaf.h"

void UInventoryLeaf::ApplyFunction(FFuncType Func)
{
	Func(this);
}

void UInventoryLeaf::ApplyWorkFunction()
{
	CallWorkFunction(this);
}

void UInventoryLeaf::Test_Function_Leaf(UInventoryCompositeBase* Composite)
{
	UE_LOG(LogTemp, Warning, TEXT("Test_Function_Leaf()"));
}

