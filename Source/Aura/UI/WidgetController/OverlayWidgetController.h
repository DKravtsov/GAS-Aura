// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "GameplayTagContainer.h"
#include "OverlayWidgetController.generated.h"

USTRUCT(BlueprintType)
struct FUIWidgetRow : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FGameplayTag MessageTag;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FText Message;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSoftClassPtr<class UAuraUserWidget> MessageWidget;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSoftObjectPtr<UTexture2D> Image;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnReceiveUIMessageSignature, const FUIWidgetRow&, UIWidgetRow);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPlayerLevelChangedSignature, const int32, NewValue, const bool, bLevelUp);



/**
 *
 */
UCLASS(BlueprintType, Blueprintable)
class AURA_API UOverlayWidgetController : public UAuraWidgetController
{
    GENERATED_BODY()

public:

    UPROPERTY(BlueprintAssignable, Category="GAS|Attributes")
    FOnAttributeChangedSignature OnAttributeChanged;

    UPROPERTY(BlueprintAssignable, Category="GAS|Messages")
    FOnReceiveUIMessageSignature OnReceiveUIMessage;

    UPROPERTY(BlueprintAssignable, Category="GAS|Player Stats")
    FOnFloatStatChangedSignature OnXPPercentChanged;

    UPROPERTY(BlueprintAssignable, Category="GAS|Player Stats")
    FOnPlayerLevelChangedSignature OnLevelChanged;

protected:

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget Data", meta = (RequiredAssetDataTags = "RowStructure=/Script/Aura.UIWidgetRow"))
    TObjectPtr<UDataTable> MessageWidgetDataTable;

private:
    bool bForceInitializeStartupAbilities = false;

public:

    virtual void BroadcastInitialValues() override;

    virtual void BindCallbacks() override;

protected:

    void AttributeChanged(const struct FOnAttributeChangeData& Data) const;

    template<class T>
    static T* GetDataTableRowByTag(UDataTable* DataTable, const FGameplayTag& Tag)
    {
        if (DataTable == nullptr)
            return nullptr;
        return DataTable->FindRow<T>(Tag.GetTagName(), TEXT("GetDataTableRowByTag"));
    }

private:

    void BroadcastMessagesByTags(const FGameplayTagContainer& AssetTags) const;
};
