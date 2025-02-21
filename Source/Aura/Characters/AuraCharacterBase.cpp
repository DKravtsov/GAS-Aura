// Copyright 4sandwiches


#include "Characters/AuraCharacterBase.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Aura.h"
#include "AuraGameplayTags.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Kismet/GameplayStatics.h"

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
    GetAuraAbilitySystemComponent()->GrantAbilities(StartupAbilities, GetCharacterLevel());
}

FVector AAuraCharacterBase::GetCombatSocketLocation_Implementation(FGameplayTag CombatSocketTag) const
{
    if (CombatSocketTag == AuraGameplayTags::CombatSocket_Weapon)
    {
        return Weapon ? Weapon->GetSocketLocation(WeaponTipSocketName) : GetMesh()->GetSocketLocation(WeaponTipSocketName);
    }
    if (CombatSocketTag == AuraGameplayTags::CombatSocket_LeftHand)
    {
        return GetMesh()->GetSocketLocation(LeftHandSocketName);
    }
    if (CombatSocketTag == AuraGameplayTags::CombatSocket_RightHand)
    {
        return GetMesh()->GetSocketLocation(RightHandSocketName);
    }
    return GetActorLocation();
}

void AAuraCharacterBase::Die()
{
    if (HasAuthority())
    {
        Weapon->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
        
        MulticastHandleDeath();
    }
}

bool AAuraCharacterBase::IsDead_Implementation() const
{
    return bDead;
}

AActor* AAuraCharacterBase::GetAvatar_Implementation()
{
    return this;
}

void AAuraCharacterBase::MulticastHandleDeath_Implementation()
{
    bDead = true;
    
    Weapon->SetSimulatePhysics(true);
    Weapon->SetEnableGravity(true);
    Weapon->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);

    GetMesh()->SetSimulatePhysics(true);
    GetMesh()->SetEnableGravity(true);
    GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
    GetMesh()->SetCollisionResponseToChannel(ECC_WorldStatic, ECollisionResponse::ECR_Block);

    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    UGameplayStatics::PlaySoundAtLocation(GetWorld(), DeathSound, GetActorLocation());

    DissolveDeadBody();

    if (HasAuthority())
    {
        if (const auto AuraCharacter = GetInstigator<AAuraCharacterBase>(); IsValid(AuraCharacter) && !AuraCharacter->bDead)
        {
            --AuraCharacter->MinionCount;
            AuraCharacter->RemovedMinion(this);
        }
    }

    OnDeath();

    DetachFromControllerPendingDestroy();
}

APawn* AAuraCharacterBase::SummonMinion_Implementation(TSubclassOf<APawn> MinionClass, FVector Location, FRotator Rotation)
{
    check(HasAuthority());
    
    if (APawn* NewMinion = UAIBlueprintHelperLibrary::SpawnAIFromClass(GetWorld(), MinionClass, nullptr, Location, Rotation, true))
    {
        NewMinion->SetInstigator(this);
        ++MinionCount;
        AddedMinion(NewMinion);
        return NewMinion;
    }
    return nullptr;
}

int32 AAuraCharacterBase::GetCharacterLevel() const
{
    return Execute_BP_GetCharacterLevel(this);
}
