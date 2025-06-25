// Copyright 4sandwiches


#include "Widgets/Composite/InventoryCompositeBase.h"

void UInventoryCompositeBase::Collapse()
{
	if (IsVisible())
	{
		SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UInventoryCompositeBase::Expand()
{
	if (!IsVisible())
	{
		SetVisibility(DefaultVisibility);
	}
}

void UInventoryCompositeBase::CallWorkFunction(UInventoryCompositeBase* Composite)
{
	if (UFunction* Func = WorkFunction.ResolveMember<UFunction>(GetClass()))
	{
		// ProcessEvent deals with raw memory, so let's set up some memory to be used
		// This is just an anonymous struct, the syntax looks kind of funky though
		struct
		{
			UInventoryCompositeBase* Comp;
		} Args = {Composite};
		
		// This will collect our results
		FStructOnScope FuncParam(Func);

		// Call our function with our parameters
		this->ProcessEvent(Func, &Args);

		UE_LOG(LogTemp, Display, TEXT("Called function '%s' with '%s'"),
			*Func->GetDisplayNameText().ToString(), *GetNameSafe(Composite));
	}
}

void UInventoryCompositeBase::Test_Function_One(UInventoryCompositeBase* Composite)
{
	UE_LOG(LogTemp, Warning, TEXT("Test_Function_One()"));
}

void UInventoryCompositeBase::Test_Function_Two(UInventoryCompositeBase* Composite)
{
	UE_LOG(LogTemp, Warning, TEXT("Test_Function_Two()"));
}
