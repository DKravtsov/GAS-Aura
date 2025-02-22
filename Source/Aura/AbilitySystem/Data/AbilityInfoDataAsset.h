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

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability", meta = (Categories = "Abilities"))
	FGameplayTag AbilityTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability")
	TSubclassOf<class UGameplayAbility> AbilityClass;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Assets")
	TSoftObjectPtr<class UTexture2D> IconImage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Assets")
	TSoftObjectPtr<class UMaterialInterface> BackgroundImage;

    UPROPERTY(BlueprintReadOnly, Category = "Tags", meta = (Categories = "InputTag"))
	FGameplayTag InputTag;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (Categories = "Cooldown"))
	FGameplayTag CooldownTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Requirements")
	int32 LevelRequirement = 1;

	/* Dynamic Data */
	
	UPROPERTY(BlueprintReadOnly, Transient)
	FGameplayTag StatusTag;

	UPROPERTY(BlueprintReadOnly, Transient)
	int32 SpellLevel;

	UPROPERTY(BlueprintReadOnly, Transient)
	FText Description;

	UPROPERTY(BlueprintReadOnly, Transient)
	FText NextLevelDescription;


public:

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
