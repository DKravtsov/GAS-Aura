// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "NativeGameplayTags.h"
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

	const FGameplayTag& GetFragmentTag() const {return FragmentTag;}
	void SetFragmentTag(const FGameplayTag& NewFragmentTag) {FragmentTag = NewFragmentTag;}

private:

	UPROPERTY(EditAnywhere, Category = "Inventory", meta=(Categories="FragmentTag"))
	FGameplayTag FragmentTag;
};

USTRUCT(BlueprintType)
struct FInventoryItemDescriptionFragment: public FInventoryItemFragment
{
	GENERATED_BODY()

	virtual void Assimilate(class UInventoryCompositeBase* Composite) const;

protected:

	bool MatchesWidgetTag(UInventoryCompositeBase* Composite) const;
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

	FInventoryItemGridFragment();

	FIntPoint GetGridSize() const {return GridSize;}
	void SetGridSize(const FIntPoint& NewGridSize) {GridSize = NewGridSize;}

	float GetGridPadding() const {return GridPadding;}
	void SetGridPadding(float NewGridPadding) {GridPadding = NewGridPadding;}
};

USTRUCT(BlueprintType)
struct FInventoryItemImageFragment: public FInventoryItemDescriptionFragment
{
	GENERATED_BODY()
private:

	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSoftObjectPtr<UTexture2D> Icon;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	FVector2D IconDimensions{32,32};

public:

	FInventoryItemImageFragment();

	const TSoftObjectPtr<UTexture2D>& GetIcon() const {return Icon;}
	TSoftObjectPtr<UTexture2D>& GetIconMutable() {return Icon;}

	//~ Begin of FInventoryItemDescriptionFragment interface
	virtual void Assimilate(UInventoryCompositeBase* Composite) const override;
	//~ End of FInventoryItemDescriptionFragment interface
};

USTRUCT(BlueprintType)
struct FInventoryItemTextFragment: public FInventoryItemDescriptionFragment
{
	GENERATED_BODY()
private:

	UPROPERTY(EditAnywhere, Category = "Inventory")
	FText FragmentText;

public:

	const FText& GetFragmentText() const {return FragmentText;}
	void SetFragmentText(const FText& NewFragmentText) {FragmentText = NewFragmentText;}

	//~ Begin of FInventoryItemDescriptionFragment interface
	virtual void Assimilate(UInventoryCompositeBase* Composite) const override;
	//~ End of FInventoryItemDescriptionFragment interface
};

USTRUCT(BlueprintType)
struct FInventoryItemStackableFragment: public FInventoryItemFragment
{
	GENERATED_BODY()
private:

	// How many stacks can one space in the inventory hold
	UPROPERTY(EditAnywhere, Category = "Inventory")
	int32 MaxStackSize = 1;

	// How many stacks should I get when I pick this item up
	UPROPERTY(EditAnywhere, Category = "Inventory")
	int32 StackCount = 1;

public:

	FInventoryItemStackableFragment();

	int32 GetMaxStackSize() const {return MaxStackSize;}
	int32 GetStackCount() const {return StackCount;}
	void SetStackCount(int32 NewStackCount) {StackCount = NewStackCount;}

};

USTRUCT(BlueprintType)
struct FInventoryItemConsumableFragment: public FInventoryItemFragment
{
	GENERATED_BODY()

	// Real consumption should be implemented by the real project by inheriting from this fragment and overriding this method
	virtual void OnConsume(const APlayerController* PC) {}
};

namespace InventoryFragmentTags
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(FragmentTag_Grid);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(FragmentTag_Image);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(FragmentTag_Stackable);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(FragmentTag_Consumable);
}