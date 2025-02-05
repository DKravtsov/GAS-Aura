// Copyright 4sandwiches


#include "Characters/AuraEnemyCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Aura.h"

AAuraEnemyCharacter::AAuraEnemyCharacter()
{
}

void AAuraEnemyCharacter::HighlightActor()
{
    if (USkeletalMeshComponent* MyMesh = GetMesh())
    {
        MyMesh->SetRenderCustomDepth(true);
        MyMesh->SetCustomDepthStencilValue(250);
    }
    if (Weapon)
    {
        Weapon->SetRenderCustomDepth(true);
        Weapon->SetCustomDepthStencilValue(CUSTOM_DEPTH_STENCIL_RED);
    }
}

void AAuraEnemyCharacter::UnhighlightActor()
{
    if (USkeletalMeshComponent* MyMesh = GetMesh())
    {
        MyMesh->SetRenderCustomDepth(false);
    }
    if (Weapon)
    {
        Weapon->SetRenderCustomDepth(false);
    }
}
