// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "AbilityInfoDataAsset.generated.h"

USTRUCT(BlueprintType)
struct FAuraAbilityInfo
{
	GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (Categories = "Abilities"))
	FGameplayTag AbilityTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText AbilityName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText Description;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftObjectPtr<class UTexture2D> IconImage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftObjectPtr<class UMaterialInterface> BackgroundImage;

    UPROPERTY(BlueprintReadOnly, meta = (Categories = "InputTag"))
	FGameplayTag InputTag;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (Categories = "Cooldown"))
	FGameplayTag CooldownTag;

	FORCEINLINE bool operator==(const FGameplayTag& Tag) const
	{
		return AbilityTag.MatchesTagExact(Tag);
	}
};

/**
 * 
 */
UCLASS()
class AURA_API UAbilityInfoDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FAuraAbilityInfo> AbilityInfos;

public:

	FAuraAbilityInfo FindAbilityInfoByTag(const FGameplayTag& Tag, bool bLogNotFound = false) const;
};
