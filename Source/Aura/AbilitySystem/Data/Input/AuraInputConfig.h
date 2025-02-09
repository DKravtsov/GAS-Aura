// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "AuraInputConfig.generated.h"


USTRUCT(BlueprintType)
struct FAuraInputAction
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly)
    const class UInputAction* InputAction = nullptr;

    UPROPERTY(EditDefaultsOnly, meta = (Categories = "InputTag"))
    FGameplayTag InputTag;


public:

    bool IsValid() const
    {
        return InputTag.IsValid() && InputAction != nullptr;
    }

    FORCEINLINE bool operator==(const FGameplayTag& Tag) const
    {
        return IsValid() && InputTag.MatchesTagExact(Tag);
    }
};

/**
 *
 */
UCLASS()
class AURA_API UAuraInputConfig : public UDataAsset
{
    GENERATED_BODY()

public:

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (TitleProperty = "InputTag"))
    TArray<FAuraInputAction> AbilityInputActions;

public:

    const UInputAction* FindInputActionByTag(const FGameplayTag& InputTag, bool bLogNotFound = false) const;
};
