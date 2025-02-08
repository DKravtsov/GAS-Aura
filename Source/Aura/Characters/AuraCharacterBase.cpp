// Copyright 4sandwiches


#include "Characters/AuraCharacterBase.h"
#include "Components/SkeletalMeshComponent.h"
#include "AbilitySystemComponent.h"
#include "Aura.h"

AAuraCharacterBase::AAuraCharacterBase()
{
    PrimaryActorTick.bCanEverTick = false;

    Weapon = CreateDefaultSubobject<USkeletalMeshComponent>("WeaponMesh");
    Weapon->SetupAttachment(GetMesh(), FName("WeaponHandSocket"));
    Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AAuraCharacterBase::BeginPlay()
{
    Super::BeginPlay();

}

void AAuraCharacterBase::InitializeDefaultAttributes()
{
    if (!IsValid(DefaultPrimaryAttributes))
    {
        FString Msg = FString::Printf(TEXT("ERROR: DefaultPrimaryAttributes is not specified for %s"), *GetNameSafe(this));
        ShowDebugMessage(-1, 30.f, FColor::Red, Msg);
        UE_LOG(LogTemp, Error, TEXT("%s"), *Msg);
        return;
    }

    const float EffectLevel = 1.f;
    ApplyGameplayEffectToSelf(DefaultPrimaryAttributes, EffectLevel);

    if (IsValid(DefaultSecondaryAttributes))
    {
        ApplyGameplayEffectToSelf(DefaultSecondaryAttributes, EffectLevel);
        return;
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("ERROR: DefaultSecondaryAttributes is not specified for %s"), *GetNameSafe(this));
    }
}

void AAuraCharacterBase::ApplyGameplayEffectToSelf(TSubclassOf<class UGameplayEffect> Effect, float EffectLevel)
{
    check(Effect != nullptr);
    check(GetAbilitySystemComponent() != nullptr);

    auto EffectContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
    EffectContextHandle.AddSourceObject(this);
    auto SpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(Effect, EffectLevel, EffectContextHandle);
    GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
}

UAbilitySystemComponent* AAuraCharacterBase::GetAbilitySystemComponent() const
{
    return AbilitySystemComponent;
}

