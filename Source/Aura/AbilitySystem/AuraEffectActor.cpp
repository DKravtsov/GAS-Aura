// Copyright 4sandwiches


#include "AbilitySystem/AuraEffectActor.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"

AAuraEffectActor::AAuraEffectActor()
{
    PrimaryActorTick.bCanEverTick = false;
    bReplicates = true;

    SetRootComponent(CreateDefaultSubobject<USceneComponent>("Root"));
}

void AAuraEffectActor::BeginPlay()
{
    Super::BeginPlay();

    if (GameplayEffectClass == nullptr)
    {
        UE_LOG(LogTemp, Error, TEXT("%s: GameplayEffectClass is not specified."), *GetNameSafe(this));
        return;
    }
}

void AAuraEffectActor::NotifyActorBeginOverlap(AActor* OtherActor)
{
    Super::NotifyActorBeginOverlap(OtherActor);

    if (bIgnoreEnemies && OtherActor->ActorHasTag("Enemy"))
    {
        return;
    }
    if (bIgnorePlayers && OtherActor->ActorHasTag("Player"))
    {
        return;
    }

    if (EffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
    {
        ApplyEffectToTarget(OtherActor);
    }
}

void AAuraEffectActor::NotifyActorEndOverlap(AActor* OtherActor)
{
    Super::NotifyActorEndOverlap(OtherActor);

    if (bIgnoreEnemies && OtherActor->ActorHasTag("Enemy"))
    {
        return;
    }
    if (bIgnorePlayers && OtherActor->ActorHasTag("Player"))
    {
        return;
    }

    if (EffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
    {
        ApplyEffectToTarget(OtherActor);
    }
    if (EffectRemovalPolicy == EEffectRemovalPolicy::RemoveOnEndOverlap)
    {
        FActiveGameplayEffectHandle ActiveEffectHandle;
        if (ActiveEffectsMap.RemoveAndCopyValue(OtherActor, ActiveEffectHandle))
        {
            auto TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor);
            if (IsValid(TargetASC))
            {
                TargetASC->RemoveActiveGameplayEffect(ActiveEffectHandle, 1);
            }
        }
    }
}

bool AAuraEffectActor::ApplyEffectToTarget(AActor* TargetActor)
{
    if (auto TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor))
    {
        checkf(GameplayEffectClass != nullptr, TEXT("%s: GameplayEffectClass is not specified."), *GetNameSafe(this));

        FGameplayEffectContextHandle EffectContextHandle = TargetASC->MakeEffectContext();
        EffectContextHandle.AddSourceObject(this);

        const FGameplayEffectSpecHandle SpecHandle = TargetASC->MakeOutgoingSpec(GameplayEffectClass, GameplayEffectLevel, EffectContextHandle);
        const bool bInfinite = SpecHandle.Data->Def->DurationPolicy == EGameplayEffectDurationType::Infinite;
 
        FActiveGameplayEffectHandle ActiveEffectHandle = TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);

        if (bInfinite && EffectRemovalPolicy == EEffectRemovalPolicy::RemoveOnEndOverlap)
        {
            ActiveEffectsMap.Add(TargetActor, ActiveEffectHandle);
        }

        if (bDestroyAfterApplying && !bInfinite)
        {
            SetActorEnableCollision(false);
            SetLifeSpan(0.01f);
        }

        return true;
    }

    return false;
}


