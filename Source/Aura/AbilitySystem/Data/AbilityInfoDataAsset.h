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

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability", meta = (Categories = "Cooldown"))
	FGameplayTag CooldownTag;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability", meta = (Categories = "Abilities.Type"))
	FGameplayTag AbilityTypeTag;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Assets")
	TSoftObjectPtr<class UTexture2D> IconImage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Assets")
	TSoftObjectPtr<class UMaterialInterface> BackgroundImage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Requirements")
	int32 LevelRequirement = 1;

protected:
	
	/* Dynamic Data */

	UPROPERTY(BlueprintReadOnly, Transient)
	FGameplayTag StatusTag;

    UPROPERTY(BlueprintReadOnly, Transient)
	FGameplayTag InputTag;

	UPROPERTY(BlueprintReadOnly, Transient)
	int32 SpellLevel = 0;

	UPROPERTY(BlueprintReadOnly, Transient)
	uint32 bStaticData : 1 = false;

	UPROPERTY(BlueprintReadOnly, Transient)
	uint32 bStatus : 1 = false;
	
	UPROPERTY(BlueprintReadOnly, Transient)
	uint32 bInputTag : 1 = false;

	UPROPERTY(BlueprintReadOnly, Transient)
	uint32 bLevel : 1 = false;

public:

	FAuraAbilityInfo() = default;
	FAuraAbilityInfo(const FAuraAbilityInfo& Other) = default;
	FAuraAbilityInfo(FAuraAbilityInfo&& Other) = default;
	FAuraAbilityInfo(const FAuraAbilityInfo& Other, const bool bWithStaticData)
		: FAuraAbilityInfo(Other)
	{
		bStaticData = bWithStaticData;
	}

	FAuraAbilityInfo& operator=(const FAuraAbilityInfo& Other) = default;
	FAuraAbilityInfo& operator=(FAuraAbilityInfo&& Other) = default;

	FORCEINLINE bool operator==(const FGameplayTag& Tag) const
	{
		return AbilityTag.MatchesTagExact(Tag);
	}

	const FGameplayTag& GetInputTag() const { return InputTag; }
	const FGameplayTag& GetStatus() const { return StatusTag; }
	int32 GetSpellLevel() const {return SpellLevel; }

	void SetStatus(const FGameplayTag& NewStatus) { StatusTag = NewStatus; bStatus = true; }
	void SetInputTag(const FGameplayTag& NewInputTag) { InputTag = NewInputTag; bInputTag = true; }
	void SetSpellLevel(const int32 NewLevel) { SpellLevel = NewLevel; bLevel = true; }
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
