// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayEffectTypes.h"
#include "AuraEffectActor.generated.h"

UENUM()
enum class EEffectApplicationPolicy
{
    ApplyOnOverlap,
    ApplyOnEndOverlap,
    DoNotApply,
    MAX UMETA(Hidden)
};

UENUM()
enum class EEffectRemovalPolicy
{
    RemoveOnEndOverlap,
    DoNotRemove,
    MAX UMETA(Hidden)
};

UCLASS()
class AURA_API AAuraEffectActor : public AActor
{
    GENERATED_BODY()

protected:

    UPROPERTY(EditAnywhere, Category = "Effects", BlueprintReadOnly)
    TSubclassOf<class UGameplayEffect> GameplayEffectClass;

    UPROPERTY(EditAnywhere, Category = "Effects", BlueprintReadOnly)
    EEffectApplicationPolicy EffectApplicationPolicy = EEffectApplicationPolicy::ApplyOnOverlap;

    UPROPERTY(EditAnywhere, Category = "Effects", BlueprintReadOnly)
    EEffectRemovalPolicy EffectRemovalPolicy = EEffectRemovalPolicy::RemoveOnEndOverlap;

    UPROPERTY(EditAnywhere, Category = "Effects", BlueprintReadWrite)
    float GameplayEffectLevel = 1.f;

    UPROPERTY(EditAnywhere, Category = "Effects", BlueprintReadOnly)
    bool bDestroyAfterApplying = false;

    UPROPERTY(EditAnywhere, Category = "Effects", BlueprintReadOnly)
    bool bIgnoreEnemies = true;

    UPROPERTY(EditAnywhere, Category = "Effects", BlueprintReadOnly)
    bool bIgnorePlayers = false;

    UPROPERTY()
    TMap<AActor*, FActiveGameplayEffectHandle> ActiveEffectsMap;

public:

    AAuraEffectActor();

    virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

    virtual void NotifyActorEndOverlap(AActor* OtherActor) override;

protected:

    virtual void BeginPlay() override;

    UFUNCTION(BlueprintCallable)
    bool ApplyEffectToTarget(AActor* TargetActor);


};
