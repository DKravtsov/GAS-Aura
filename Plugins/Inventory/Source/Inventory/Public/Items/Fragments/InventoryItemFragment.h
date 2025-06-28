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
struct FInventoryItemDescriptionFragment: public FInventoryItemFragment
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
struct FInventoryItemNumericValueFragment: public FInventoryItemDescriptionFragment
{
	GENERATED_BODY()
private:

	UPROPERTY(EditAnywhere, Category = "Inventory")
	FText LabelText;

	UPROPERTY(VisibleInstanceOnly, Category = "Inventory")
	float Value = 0.f;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	float MinValue = 0.f;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	float MaxValue = 1.f;

	UPROPERTY(EditAnywhere, Category = "Inventory", AdvancedDisplay)
	int32 MinFractionalDigits = 0;

	UPROPERTY(EditAnywhere, Category = "Inventory", AdvancedDisplay)
	int32 MaxFractionalDigits = 1;

	UPROPERTY(EditAnywhere, Category = "Inventory", AdvancedDisplay)
	uint8 bCollapseLabel:1 = false;

	UPROPERTY(EditAnywhere, Category = "Inventory", AdvancedDisplay)
	uint8 bCollapseValue:1 = false;

	uint8 bRandomized:1 = false;

protected:

	void SetValue(float NewValue) {Value = NewValue;}
	float GetValue() const {return Value;}

public:

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

/*
 *    Consumable  
 */

USTRUCT(BlueprintType)
struct FInventoryConsumeModifier : public FInventoryItemNumericValueFragment 
{
	GENERATED_BODY()
	
	virtual void OnConsume(const APlayerController* PC) const {}
};

USTRUCT(BlueprintType)
struct FInventoryItemConsumableFragment: public FInventoryItemDescriptionFragment
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
struct FInventoryEquipModifier : public FInventoryItemNumericValueFragment 
{
	GENERATED_BODY()
	
	virtual void OnEquip(const APlayerController* PC) const {}
	virtual void OnUnequip(const APlayerController* PC) const {}
};

USTRUCT(BlueprintType)
struct FInventoryItemEquipmentFragment: public FInventoryItemDescriptionFragment
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