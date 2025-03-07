// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AuraBlueprintFunctionLibrary.generated.h"

/**
 *
 */
UCLASS()
class AURA_API UAuraBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:

    UFUNCTION(BlueprintPure, Category = "AuraAbilitySystem|WidgetController", BlueprintCosmetic, meta = (WorldContext = "WorldContextObject"))
    static class UOverlayWidgetController* GetOverlayWidgetController(const UObject* WorldContextObject);

    UFUNCTION(BlueprintPure, Category = "AuraAbilitySystem|WidgetController", BlueprintCosmetic, meta = (WorldContext = "WorldContextObject"))
    static class UAttributeMenuWidgetController* GetAttributeMenuWidgetController(const UObject* WorldContextObject);

    UFUNCTION(BlueprintPure, Category = "AuraAbilitySystem|WidgetController", BlueprintCosmetic, meta = (WorldContext = "WorldContextObject"))
    static class USpellMenuWidgetController* GetSpellMenuWidgetController(const UObject* WorldContextObject);

    UFUNCTION(BlueprintPure, Category = "AuraAbilitySystem|CharacterClassDefaults", meta = (WorldContext = "WorldContextObject"))
    static class UCharacterClassInfo* GetCharacterClassInfo(const UObject* WorldContextObject);

    UFUNCTION(BlueprintPure, Category = "AuraAbilitySystem|CharacterClassDefaults", meta = (WorldContext = "WorldContextObject"))
    static class UAbilityInfoDataAsset* GetAbilityInfo(const UObject* WorldContextObject);

    UFUNCTION(BlueprintPure, Category = "AuraAbilitySystem|LootTiers", meta = (WorldContext = "WorldContextObject"))
    static class ULootTiersDataAsset* GetLootTiers(const UObject* WorldContextObject);



    UFUNCTION(BlueprintPure, Category = "AuraAbilitySystem|CharacterInfo")
    static int32 GetCharacterLevel(const AActor* Actor);


    UFUNCTION(BlueprintPure, Category = "AuraAbilitySystem|GameplayEffects")
    static bool IsBlockedHit(const struct FGameplayEffectContextHandle& EffectContextHandle);

    UFUNCTION(BlueprintPure, Category = "AuraAbilitySystem|GameplayEffects")
    static bool IsCriticalHit(const FGameplayEffectContextHandle& EffectContextHandle);

    UFUNCTION(BlueprintPure, Category = "AuraAbilitySystem|GameplayEffects")
    static bool IsSuccessfulDebuff(const FGameplayEffectContextHandle& EffectContextHandle);

    using FDebuffParams = TTuple<float/*Damage*/, float/*Duration*/, float/*Frequency*/, FGameplayTag/*DamageType*/>;
    static FDebuffParams GetDebuffParams(const FGameplayEffectContextHandle& EffectContextHandle);

    UFUNCTION(BlueprintPure, Category = "AuraAbilitySystem|GameplayEffects")
    static float GetDebuffDamage(const FGameplayEffectContextHandle& EffectContextHandle);

    UFUNCTION(BlueprintPure, Category = "AuraAbilitySystem|GameplayEffects")
    static float GetDebuffDuration(const FGameplayEffectContextHandle& EffectContextHandle);

    UFUNCTION(BlueprintPure, Category = "AuraAbilitySystem|GameplayEffects")
    static float GetDebuffFrequency(const FGameplayEffectContextHandle& EffectContextHandle);

    UFUNCTION(BlueprintPure, Category = "AuraAbilitySystem|GameplayEffects")
    static FGameplayTag GetDamageTypeFromEffectContext(const FGameplayEffectContextHandle& EffectContextHandle);

    UFUNCTION(BlueprintPure, Category = "AuraAbilitySystem|GameplayEffects")
    static FVector GetDeathImpulse(const FGameplayEffectContextHandle& EffectContextHandle);

    UFUNCTION(BlueprintPure, Category = "AuraAbilitySystem|GameplayEffects")
    static FVector GetKnockBackImpulse(const FGameplayEffectContextHandle& EffectContextHandle);

    UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystem|GameplayEffects")
    static void SetIsBlockedHit(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, bool bBlockedHit);

    UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystem|GameplayEffects")
    static void SetIsCriticalHit(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, bool bCriticalHit);

    UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystem|GameplayEffects")
    static void SetIsSuccessfulDebuff(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, bool bDebuff);

    UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystem|GameplayEffects")
    static void SetupDebuffParams(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle,
        float InDamage, float InDuration, float InFrequency, const FGameplayTag& InDamageType);

    UFUNCTION(BlueprintPure, Category = "Actor")
    static APawn* FindNearestAlivePawn(FVector Origin, const TArray<APawn*>& ActorsToCheck, float& Distance);
    
    UFUNCTION(BlueprintCallable, Category = "Aura|Combat", meta = (WorldContext = "WorldContextObject", AutoCreateRefTerm="IgnoreActors"))
    static void GetAllLivePlayersInRadius(const UObject* WorldContextObject, TArray<AActor*>& LivePlayers, const float Radius, const FVector& Origin, const TArray<AActor*>& IgnoreActors);

    UFUNCTION(BlueprintPure, Category = "Aura|Combat")
    static bool AreFriendly(const AActor* A, const AActor* B);

    // Returns random integer respecting given weights, in range [0..Weights.Length)
    UFUNCTION(Category = "Math|Random", BlueprintPure)
    static int32 RandomIntWithWeights(TArray<float> Weights);

    UFUNCTION(BlueprintCallable, Category = "Aura|Combat", meta=(ReturnDisplayName="EffectContext"))
    static FGameplayEffectContextHandle ApplyDamageEffect(const struct FDamageEffectParams& Params);

    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Aura|Combat", meta=(ReturnDisplayName="EffectContexts", AutoCreateRefTerm="IgnoreActors", AdvancedDisplay="bKnockBackPitchOverride,KnockBackPitch"))
    static TArray<FGameplayEffectContextHandle> ApplyRadialDamageEffect(const FDamageEffectParams& Params,
        const FVector& Origin, float DamageInnerRadius, float DamageOuterRadius, float DamageFalloff, const TArray<AActor*>& IgnoreActors,
        bool bKnockBackPitchOverride = false, const float KnockBackPitch = 0.f);

    UFUNCTION(BlueprintCallable, Category = "Aura|Combat", meta=(AdvancedDisplay="bKnockBackPitchOverride,KnockBackPitch"))
    static void AddKnockBackParams(const AActor* TargetActor, const FVector& Origin, UPARAM(ref) FDamageEffectParams& Params, bool bKnockBackPitchOverride = false, const float KnockBackPitch = 0.f);

    static void ScaleDamageEffectParams(FDamageEffectParams& Params, const float Scale);

    UFUNCTION(BlueprintPure, Category = "Math")
    static TArray<FVector> GetUniformSpreadOfDirections(const FVector& Forward, const float SpreadAngle, int32 NumDirections);

    UFUNCTION(BlueprintPure, Category = "Math")
    static float FRandInRange(const FVector2D& MinMax);
    static float RandInRange(const FIntVector2& MinMax);

    
    UFUNCTION(BlueprintPure, Category = "GameplayTags")
    static void GetActorOwnedGameplayTagContainer(AActor* Actor, FGameplayTagContainer& OutTagContainer);
    
    UFUNCTION(BlueprintPure, Category = "GameplayTags")
    static bool DoesActorHaveAllGameplayTags(AActor* Actor, const FGameplayTagContainer& TagContainer);

    UFUNCTION(BlueprintPure, Category = "GameplayTags")
    static bool DoesActorHaveAnyGameplayTags(AActor* Actor, const FGameplayTagContainer& TagContainer);

    UFUNCTION(BlueprintPure, Category = "GameplayTags")
    static bool DoesActorHaveGameplayTag(AActor* Actor, FGameplayTag Tag);

    UFUNCTION(BlueprintCallable, Category = "GameplayTags")
    static void AddGameplayTagToActor(AActor* Actor, FGameplayTag Tag);

    UFUNCTION(BlueprintCallable, Category = "GameplayTags")
    static void RemoveGameplayTagFromActor(AActor* Actor, FGameplayTag Tag);

    UFUNCTION(BlueprintCallable, Category = "GameplayTags", meta = (DisplayName = "Add Gameplay Tag to Actor (Replicated)"))
    static void AddGameplayTagToActor_Replicated(AActor* Actor, FGameplayTag Tag);

    UFUNCTION(BlueprintCallable, Category = "GameplayTags", meta = (DisplayName = "Remove Gameplay Tag from Actor (Replicated)"))
    static void RemoveGameplayTagFromActor_Replicated(AActor* Actor, FGameplayTag Tag);

    UFUNCTION(BlueprintPure)
    static float GetValueAtLevel(const struct FScalableFloat& InVariable, const int32 InLevel);
};