// Copyright 4sandwiches


#include "AbilitySystem/AuraEffectActor.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"

AAuraEffectActor::AAuraEffectActor()
{
    PrimaryActorTick.bCanEverTick = false;

    SetRootComponent(CreateDefaultSubobject<USceneComponent>("Root"));
}

void AAuraEffectActor::BeginPlay()
{
    Super::BeginPlay();

    if (InstantGameplayEffectClass == nullptr)
    {
        UE_LOG(LogTemp, Error, TEXT("%s: InstantGameplayEffectClass is not specified."), *GetNameSafe(this));
        return;
    }
}

bool AAuraEffectActor::ApplyEffectToTarget(AActor* TargetActor)
{
    if (auto TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor))
    {
        checkf(InstantGameplayEffectClass != nullptr, TEXT("%s: InstantGameplayEffectClass is not specified."), *GetNameSafe(this));

        FGameplayEffectContextHandle EffectContextHandle = TargetASC->MakeEffectContext();
        EffectContextHandle.AddSourceObject(this);

        const FGameplayEffectSpecHandle SpecHandle = TargetASC->MakeOutgoingSpec(InstantGameplayEffectClass, InstantGameplayEffectLevel, EffectContextHandle);

        TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);

        return true;
    }

    return false;
}


