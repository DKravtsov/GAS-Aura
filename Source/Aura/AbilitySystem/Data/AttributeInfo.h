// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "AttributeSet.h"
#include "AttributeInfo.generated.h"

USTRUCT(BlueprintType)
struct FAuraAttributeInfo
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (Categories = "Attributes"))
    FGameplayTag AttributeTag;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FText AttributeName;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FText Description;

    UPROPERTY(BlueprintReadOnly, Transient)
    float Value = 0.f;

public:

    FORCEINLINE bool operator==(const FGameplayTag& Tag) const
    {
        return AttributeTag.MatchesTagExact(Tag);
    }
};


/**
 *
 */
UCLASS()
class AURA_API UAttributeInfo : public UDataAsset
{
    GENERATED_BODY()
public:

    //UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (TitleProperty = "AttributeName"))
    //TArray<FAuraAttributeInfo> AttributeInfo;

    //UPROPERTY(EditDefaultsOnly, meta = (TitleProperty = "AttributeName", ForceInlineRow))
    TMap<struct FGameplayAttribute, FGameplayTag> AttributeMap;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ForceInlineRow))
    TMap<struct FGameplayAttribute, FAuraAttributeInfo> AttributeInfoMap;

public:

    FAuraAttributeInfo FindAttributeInfoByTag(const FGameplayTag& AttributeTag, bool bLogNotFound = false) const;

    FAuraAttributeInfo FindAttributeInfo(const FGameplayAttribute& Attribute, bool bLogNotFound = false) const;
};
