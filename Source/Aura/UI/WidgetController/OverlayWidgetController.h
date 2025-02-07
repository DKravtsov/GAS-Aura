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

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAttributeChangedSignature, struct FGameplayAttribute, Attribute, float, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnReceiveUIMessageSignature, const FUIWidgetRow&, UIWidgetRow);



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

protected:

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget Data", meta = (RequiredAssetDataTags = "RowStructure=/Script/Aura.UIWidgetRow"))
    TObjectPtr<UDataTable> MessageWidgetDataTable;

public:

    void BroadcastInitialValues() override;

    void BindCallbacks() override;

protected:

    void AttributeChanged(const struct FOnAttributeChangeData& Data);

    template<class T>
    static T* GetDataTableRowByTag(UDataTable* DataTable, const FGameplayTag& Tag)
    {
        if (DataTable == nullptr)
            return nullptr;
        return DataTable->FindRow<T>(Tag.GetTagName(), TEXT("GetDataTableRowByTag"));
    }

private:

    void BroadcastMessagesByTags(const FGameplayTagContainer& AssetTags);
};
