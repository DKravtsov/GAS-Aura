// Copyright 4sandwiches

#pragma once

#include "DebugHelper.h"

#include "Items/InventoryItem.h"

FString GetInventoryItemId(const class UInventoryItem* Item)
{
	return Item ? Item->GetItemType().ToString() : FString(TEXT("None"));
}
namespace DebugHelper
{
	FString GetCallerPathAndOwner(const UObject* Object, const AActor* &OwnerActor, const TCHAR* Extra)
	{
		FStringBuilderBase Output;
	
		for (const UObject* Top = Object; Top && !Top->IsA<UPackage>(); Top = Top->GetOuter())
		{
			if (Output.Len() > 0)
				Output.InsertAt(0, TEXT("."));
			Output.InsertAt(0, Top->GetName());
			if (Top->IsA<AActor>())
			{
				OwnerActor = Cast<AActor>(Top);
				break;
			}
		}
		if (Extra)
			Output.AppendChar('.').Append(Extra);
		return Output.ToString();
	}

	FString GetCallerPathAndOwner(const UUserWidget* Object, const AActor*& OwnerActor, const TCHAR* Extra)
	{
		OwnerActor = Object->GetOwningPlayer();
		return FString::Printf(TEXT("%s Widget: %s"), *OwnerActor->GetName(), *Object->GetName());
	}

	FString GetObjectNetInfo(const UObject* Object)
	{
		const AActor* OwnerActor = nullptr;
		FStringBuilderBase Output;
	
		for (const UObject* Top = Object; Top && !Top->IsA<UPackage>(); Top = Top->GetOuter())
		{
			if (Output.Len() > 0)
				Output.InsertAt(0, TEXT("."));
			Output.InsertAt(0, Top->GetName());
			if (Top->IsA<AActor>())
			{
				OwnerActor = Cast<AActor>(Top);
				break;
			}
		}
		return Output.ToString();
	}
}