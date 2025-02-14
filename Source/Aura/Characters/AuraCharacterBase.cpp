// Copyright 4sandwiches


#include "Characters/AuraCharacterBase.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
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
    if (DefaultPrimaryAttributes == nullptr)
    {
        FString Msg = FString::Printf(TEXT("ERROR: DefaultPrimaryAttributes is not specified for %s"), *GetNameSafe(this));
        ShowDebugMessage(-1, 30.f, FColor::Red, Msg);
        UE_LOG(LogTemp, Error, TEXT("%s"), *Msg);
        return;
    }

    ApplyGameplayEffectToSelf(DefaultPrimaryAttributes, GetCharacterLevel());

    if (DefaultSecondaryAttributes)
    {
        ApplyGameplayEffectToSelf(DefaultSecondaryAttributes, GetCharacterLevel());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("DefaultSecondaryAttributes is not specified for %s"), *GetNameSafe(this));
    }

    if (DefaultVitalAttributes)
    {
        ApplyGameplayEffectToSelf(DefaultVitalAttributes, GetCharacterLevel());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("DefaultVitalAttributes is not specified for %s"), *GetNameSafe(this));
    }

    // Initialize other sturtup effects

    for (const auto Effect : StartupGameplayEffects)
    {
        ApplyGameplayEffectToSelf(Effect, GetCharacterLevel());
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

UAuraAbilitySystemComponent* AAuraCharacterBase::GetAuraAbilitySystemComponent() const
{
    return Cast<UAuraAbilitySystemComponent>(GetAbilitySystemComponent());
}

void AAuraCharacterBase::GrantStartupAbilities()
{
    if (!HasAuthority())
        return;
    GetAuraAbilitySystemComponent()->GrantStartupAbilities(StartupAbilities, GetCharacterLevel());
}

FVector AAuraCharacterBase::GetCombatSocketLocation_Implementation() const
{
    check(Weapon);
    return Weapon->GetSocketLocation(WeaponTipSocketName);
}

void AAuraCharacterBase::Die()
{
    if (HasAuthority())
    {
        Weapon->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);

        MulticastHandleDeath();
    }
}

void AAuraCharacterBase::MulticastHandleDeath_Implementation()
{
    Weapon->SetSimulatePhysics(true);
    Weapon->SetEnableGravity(true);
    Weapon->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);

    GetMesh()->SetSimulatePhysics(true);
    GetMesh()->SetEnableGravity(true);
    GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
    GetMesh()->SetCollisionResponseToChannel(ECC_WorldStatic, ECollisionResponse::ECR_Block);

    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    DissolveDeadBody();
}

