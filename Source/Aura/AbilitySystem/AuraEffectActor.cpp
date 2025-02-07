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

    if (EffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
    {
        const bool bApplied = ApplyEffectToTarget(OtherActor);

        if (bApplied && bDestroyAfterApplying)
        {
            SetLifeSpan(0.01f);
        }
    }
}

void AAuraEffectActor::NotifyActorEndOverlap(AActor* OtherActor)
{
    Super::NotifyActorEndOverlap(OtherActor);

    if (EffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
    {
        const bool bApplied = ApplyEffectToTarget(OtherActor);
        if (bApplied && bDestroyAfterApplying)
        {
            SetLifeSpan(0.01f);
        }
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
        const bool bInifinite = SpecHandle.Data->Def->DurationPolicy == EGameplayEffectDurationType::Infinite;

        FActiveGameplayEffectHandle ActiveEffectHandle = TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);

        if (bInifinite && EffectRemovalPolicy == EEffectRemovalPolicy::RemoveOnEndOverlap)
        {
            ActiveEffectsMap.Add(TargetActor, ActiveEffectHandle);
        }

        return true;
    }

    return false;
}


