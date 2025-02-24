// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Interface.h"
#include "CombatInterface.generated.h"

enum class EAuraCharacterClass : uint8;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDeathSignature, AActor*, DeadActor);

USTRUCT(BlueprintType)
struct FAttackInfo
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (Categories = "Montage"))
    FGameplayTag MontageTag;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<class UAnimMontage> Montage = nullptr;

    // used mostly for melee attacks
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float AttackRadius = 50.f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (Categories = "CombatSocket"))
    FGameplayTag SocketTag;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (Categories = "GameplayCue"))
    FGameplayTag ImpactGameplayCueTag;
};


// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UCombatInterface : public UInterface
{
    GENERATED_BODY()
};

/**
 *
 */
class AURA_API ICombatInterface
{
    GENERATED_BODY()

public:

    UFUNCTION(BlueprintNativeEvent)
    int32 BP_GetCharacterLevel() const;

    UFUNCTION(BlueprintNativeEvent)
    EAuraCharacterClass GetCharacterClass() const;

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, meta = (Categories = "CombatSocket"))
    FVector GetCombatSocketLocation(FGameplayTag CombatSocketTag) const;

    // Set Target Location for anim montages with MotionWarping or similar
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void UpdateFacingTarget(const FVector& TargetLocation);

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    class UAnimMontage* GetHitReactAnimMontage() const;

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void SetCombatTarget(AActor* TargetActor);

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    AActor* GetCombatTarget() const;
    
    virtual void Die() = 0;

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    bool IsDead() const;

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    AActor* GetAvatar();

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    TArray<FAttackInfo> GetAttackMontages() const;

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    class UNiagaraSystem* GetBloodEffect() const;

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    FAttackInfo GetTaggedMontageByTag(FGameplayTag MontageTag);

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    int32 GetMinionCount() const;

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    APawn* SummonMinion(TSubclassOf<APawn> MinionClass, FVector Location, FRotator Rotation);

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    int32 GetRewardXP() const;

    UFUNCTION(BlueprintNativeEvent)
    void AddXP(int32 Amount);

    UFUNCTION(BlueprintNativeEvent)
    void NotifyLevelUp();

    virtual FOnDeathSignature& GetOnDeathDelegate() = 0;
};
