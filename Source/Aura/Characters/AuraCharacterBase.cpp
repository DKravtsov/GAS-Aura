// Copyright 4sandwiches


#include "Characters/AuraCharacterBase.h"
#include "Components/SkeletalMeshComponent.h"

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

UAbilitySystemComponent* AAuraCharacterBase::GetAbilitySystemComponent() const
{
    return AbilitySystemComponent;
}

