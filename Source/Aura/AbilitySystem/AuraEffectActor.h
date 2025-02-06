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
    TSubclassOf<UGameplayEffect> GameplayEffectClass;

    UPROPERTY(EditAnywhere, Category = "Effects", BlueprintReadOnly)
    EEffectApplicationPolicy EffectApplicationPolicy = EEffectApplicationPolicy::ApplyOnOverlap;

    UPROPERTY(EditAnywhere, Category = "Effects", BlueprintReadOnly)
    EEffectRemovalPolicy EffectRemovalPolicy = EEffectRemovalPolicy::RemoveOnEndOverlap;

    UPROPERTY(EditAnywhere, Category = "Effects", BlueprintReadOnly)
    float GameplayEffectLevel = 1.f;

    UPROPERTY(EditAnywhere, Category = "Effects", BlueprintReadOnly)
    bool bDestroyAfterApplying = false;

    TMap<AActor*, FActiveGameplayEffectHandle> ActiveEffectsMap;

public:

    AAuraEffectActor();

    void NotifyActorBeginOverlap(AActor* OtherActor) override;

    void NotifyActorEndOverlap(AActor* OtherActor) override;

protected:

    void BeginPlay() override;

    UFUNCTION(BlueprintCallable)
    bool ApplyEffectToTarget(AActor* TargetActor);


};
