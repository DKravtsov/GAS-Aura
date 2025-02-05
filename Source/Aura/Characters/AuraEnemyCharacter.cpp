// Copyright 4sandwiches


#include "Characters/AuraEnemyCharacter.h"
#include "Components/CapsuleComponent.h"

AAuraEnemyCharacter::AAuraEnemyCharacter()
{
}

void AAuraEnemyCharacter::HighlightActor()
{
    GetCapsuleComponent()->SetHiddenInGame(false);
}

void AAuraEnemyCharacter::UnhighlightActor()
{
    GetCapsuleComponent()->SetHiddenInGame(true);
}
