// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
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



    UFUNCTION(BlueprintPure, Category = "AuraAbilitySystem|CharacterInfo")
    static int32 GetCharacterLevel(const AActor* Actor);


    UFUNCTION(BlueprintPure, Category = "AuraAbilitySystem|GameplayEffects")
    static bool IsBlockedHit(const struct FGameplayEffectContextHandle& EffectContextHandle);

    UFUNCTION(BlueprintPure, Category = "AuraAbilitySystem|GameplayEffects")
    static bool IsCriticalHit(const FGameplayEffectContextHandle& EffectContextHandle);

    UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystem|GameplayEffects")
    static void SetIsBlockedHit(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, bool bBlockedHit);

    UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystem|GameplayEffects")
    static void SetIsCriticalHit(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, bool bCriticalHit);

    UFUNCTION(BlueprintPure, Category = "Actor")
    static APawn* FindNearestAlivePawn(FVector Origin, const TArray<APawn*>& ActorsToCheck, float& Distance);
    
    UFUNCTION(BlueprintCallable, Category = "Aura|Combat", meta = (WorldContext = "WorldContextObject"))
    static void GetAllLivePlayersInRadius(const UObject* WorldContextObject, TArray<AActor*>& LivePlayers, const float Radius, const FVector& Origin, const TArray<AActor*>& IgnoreActors);

    UFUNCTION(BlueprintPure, Category = "Aura|Combat")
    static bool AreFriendly(const AActor* A, const AActor* B);

    // Returns random integer respecting given weights, in range [0..Weights.Length)
    UFUNCTION(Category = "Math|Random", BlueprintPure)
    static int32 RandomIntWithWeights(TArray<float> Weights);
};