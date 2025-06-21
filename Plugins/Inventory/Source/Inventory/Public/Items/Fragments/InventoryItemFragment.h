// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "InventoryItemFragment.generated.h"


USTRUCT(BlueprintType)
struct FInventoryItemFragment
{
	GENERATED_BODY()

	FInventoryItemFragment() {}
	FInventoryItemFragment(const FInventoryItemFragment&) = default;
	FInventoryItemFragment(FInventoryItemFragment&&) = default;
	FInventoryItemFragment& operator=(const FInventoryItemFragment&) = default;
	FInventoryItemFragment& operator=(FInventoryItemFragment&&) = default;
	virtual ~FInventoryItemFragment() {}

	FGameplayTag GetFragmentTag() const {return FragmentTag;}
	void SetFragmentTag(const FGameplayTag& NewFragmentTag) {FragmentTag = NewFragmentTag;}

private:

	UPROPERTY(EditAnywhere, Category = "Inventory")
	FGameplayTag FragmentTag;
};

USTRUCT(BlueprintType)
struct FInventoryItemGridFragment : public FInventoryItemFragment
{
	GENERATED_BODY()
private:
	
	UPROPERTY(EditAnywhere, Category = "Inventory")
	FIntPoint GridSize{1,1};

	UPROPERTY(EditAnywhere, Category = "Inventory")
	float GridPadding = 0.f;

public:

	FIntPoint GetGridSize() const {return GridSize;}
	void SetGridSize(const FIntPoint& NewGridSize) {GridSize = NewGridSize;}

	float GetGridPadding() const {return GridPadding;}
	void SetGridPadding(float NewGridPadding) {GridPadding = NewGridPadding;}
};