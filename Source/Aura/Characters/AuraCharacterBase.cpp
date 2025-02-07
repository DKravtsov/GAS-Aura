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

void AAuraCharacterBase::InitializePrimaryAttributes()
{
    check(IsValid(GetAbilitySystemComponent()));
    
    if (!IsValid(DefaultPrimaryAttributes))
    {
        FString Msg = FString::Printf(TEXT("ERROR: DefaultPrimaryAttributes is not specified for %s"), *GetNameSafe(this));
        ShowDebugMessage(-1, 30.f, FColor::Red, Msg);
        UE_LOG(LogTemp, Error, TEXT("%s"), *Msg);
        return;
    }

    auto EffectContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
    const float EffectLevel = 1.f;
    auto SpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(DefaultPrimaryAttributes, EffectLevel, EffectContextHandle);
    GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
}

UAbilitySystemComponent* AAuraCharacterBase::GetAbilitySystemComponent() const
{
    return AbilitySystemComponent;
}

