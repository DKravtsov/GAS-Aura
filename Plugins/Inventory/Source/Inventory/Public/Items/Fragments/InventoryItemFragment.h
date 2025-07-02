// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "NativeGameplayTags.h"
#include "StructUtils/InstancedStruct.h"
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

	virtual void Manifest() {}

private:

	UPROPERTY(EditAnywhere, Category = "Inventory", meta=(Categories="FragmentTag"))
	FGameplayTag FragmentTag;
};

USTRUCT(BlueprintType)
struct FInventoryItemPropertyFragment: public FInventoryItemFragment
{
	GENERATED_BODY()

	INVENTORY_API virtual void Assimilate(class UInventoryCompositeBase* Composite) const;

protected:

	bool MatchesWidgetTag(const UInventoryCompositeBase* Composite) const;
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
struct FInventoryItemImageFragment: public FInventoryItemPropertyFragment
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
struct FInventoryItemTextFragment: public FInventoryItemPropertyFragment
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
struct FInventoryNumericValueBase
{
	GENERATED_BODY()

	FInventoryNumericValueBase() = default;
	FInventoryNumericValueBase(const FInventoryNumericValueBase&) = default;
	FInventoryNumericValueBase(FInventoryNumericValueBase&&) = default;
	FInventoryNumericValueBase& operator=(const FInventoryNumericValueBase&) = default;
	FInventoryNumericValueBase& operator=(FInventoryNumericValueBase&&) = default;
	virtual ~FInventoryNumericValueBase() = default;

	virtual void Initialize() {}
	virtual float GetValue() const {return 0.f;}
	virtual void SetValue(float NewValue) {}
};

USTRUCT(BlueprintType)
struct FInventoryNumericValue : public FInventoryNumericValueBase
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleInstanceOnly, Category = "Inventory")
	float Value = 0.f;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	FVector2D MinMaxValue{0.f, 1.f};

	uint8 bInitialized:1 = false;

	INVENTORY_API virtual void Initialize() override;

	virtual float GetValue() const override {return Value;}
	virtual void SetValue(float NewValue) override {Value = NewValue;}

};

USTRUCT(BlueprintType)
struct FInventoryItemLabeledValueFragment: public FInventoryItemPropertyFragment
{
	GENERATED_BODY()
private:

	UPROPERTY(EditAnywhere, Category = "Inventory", meta=(EditCondition="bEnableLabel"))
	FText LabelText;

	UPROPERTY(EditAnywhere, Category = "Inventory", meta=(EditCondition="bEnableValue", ExcludeBaseStruct))
	TInstancedStruct<FInventoryNumericValueBase> NumericValue;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	FIntPoint MinMaxFractionalDigits{0, 1};

	UPROPERTY(EditAnywhere, Category = "Inventory", AdvancedDisplay, meta=(InlineEditConditionToggle))
	uint8 bEnableLabel:1 = true;

	UPROPERTY(EditAnywhere, Category = "Inventory", AdvancedDisplay, meta=(InlineEditConditionToggle))
	uint8 bEnableValue:1 = true;

protected:

	float GetValue() const {return IsValueValid() ? NumericValue.Get().GetValue() : 0.f;}

	bool IsValueValid() const {return NumericValue.IsValid();}

public:

	INVENTORY_API FInventoryItemLabeledValueFragment();

	const FText& GetLabelText() const {return LabelText;}
	void SetFragmentText(const FText& NewText) {LabelText = NewText;}

	//~ Begin of FInventoryItemDescriptionFragment interface
	INVENTORY_API virtual void Assimilate(UInventoryCompositeBase* Composite) const override;
	//~ End of FInventoryItemDescriptionFragment interface

	//~ Begin of FInventoryItemFragment interface
	INVENTORY_API virtual void Manifest() override;
	//~ End of FInventoryItemFragment interface
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
struct FInventoryItemStackableRandomFragment: public FInventoryItemStackableFragment
{
	GENERATED_BODY()
private:

	// How many stacks initially this item has
	UPROPERTY(EditAnywhere, Category = "Inventory")
	FIntPoint InitialMinMaxCount {1,1};

public:

	//~ Begin of FInventoryItemFragment interface
	INVENTORY_API virtual void Manifest() override;
	//~ End of FInventoryItemFragment interface

};

/*
 *    Consumable  
 */

USTRUCT(BlueprintType)
struct FInventoryConsumeModifier : public FInventoryItemLabeledValueFragment 
{
	GENERATED_BODY()
	
	virtual void OnConsume(const APlayerController* PC) const {}
};

USTRUCT(BlueprintType)
struct FInventoryItemConsumableFragment: public FInventoryItemPropertyFragment
{
	GENERATED_BODY()
	
private:

	UPROPERTY(EditAnywhere, Category = "Inventory", meta=(ExcludeBaseStruct))
	TArray<TInstancedStruct<FInventoryConsumeModifier>> ConsumeModifiers;

public:
	// Real consumption should be implemented by the real project by inheriting from this fragment and overriding this method
	void OnConsume(const APlayerController* PC) const;

	//~ Begin of FInventoryItemDescriptionFragment interface
	virtual void Assimilate(UInventoryCompositeBase* Composite) const override;
	//~ End of FInventoryItemDescriptionFragment interface

	//~ Begin of FInventoryItemFragment interface
	virtual void Manifest() override;
	//~ End of FInventoryItemFragment interface

};

/*
 *     Equipment
 */


USTRUCT(BlueprintType)
struct FInventoryEquipModifier : public FInventoryItemLabeledValueFragment 
{
	GENERATED_BODY()
	
	virtual void OnEquip(const APlayerController* PC) const {}
	virtual void OnUnequip(const APlayerController* PC) const {}
};

USTRUCT(BlueprintType)
struct FInventoryItemEquipmentFragment: public FInventoryItemPropertyFragment
{
	GENERATED_BODY()
	
private:

	UPROPERTY(EditAnywhere, Category="Inventory", meta=(Categories="GameItems.Equipment"))
	FGameplayTag EquipmentType;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSoftClassPtr<class AInventoryEquipActor> EquippedActorClass;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	FName SocketAttachPoint;

	UPROPERTY(EditAnywhere, Category = "Inventory", meta=(ExcludeBaseStruct))
	TArray<TInstancedStruct<FInventoryEquipModifier>> EquipModifiers;

	TWeakObjectPtr<AInventoryEquipActor> EquippedActor;

	bool bEquipped = false;

public:
	
	const FGameplayTag& GetEquipmentType() const {return EquipmentType;}
	
	void SetEquippedActor(AInventoryEquipActor* EquipActor);

	void OnEquip(const APlayerController* PC);
	void OnUnequip(const APlayerController* PC);

	//~ Begin of FInventoryItemDescriptionFragment interface
	virtual void Assimilate(UInventoryCompositeBase* Composite) const override;
	//~ End of FInventoryItemDescriptionFragment interface

	//~ Begin of FInventoryItemFragment interface
	virtual void Manifest() override;
	//~ End of FInventoryItemFragment interface

	AInventoryEquipActor* SpawnEquippedActor(USkeletalMeshComponent* ParentMesh) const;
	void DestroyEquipActor() const;
};

USTRUCT(BlueprintType)
struct FInventoryExampleEquipModifier : public FInventoryEquipModifier 
{
	GENERATED_BODY()
	
	virtual void OnEquip(const APlayerController* PC) const override;
	virtual void OnUnequip(const APlayerController* PC) const override;
};


namespace InventoryFragmentTags
{
	INVENTORY_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(FragmentTag_Grid);
	INVENTORY_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(FragmentTag_Image);
	INVENTORY_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(FragmentTag_Stackable);
	INVENTORY_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(FragmentTag_Consumable);
	INVENTORY_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(FragmentTag_ItemName);
	INVENTORY_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(FragmentTag_ItemDescription);
}