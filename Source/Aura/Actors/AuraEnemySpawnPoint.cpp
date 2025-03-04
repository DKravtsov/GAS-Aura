// Copyright 4sandwiches


#include "Actors/AuraEnemySpawnPoint.h"

#include "Characters/AuraEnemyCharacter.h"
#include "Components/CapsuleComponent.h"

AAuraEnemySpawnPoint::AAuraEnemySpawnPoint(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCapsuleComponent>("SceneComp"))
{
#if WITH_EDITORONLY_DATA
	CapsuleComponent = Cast<UCapsuleComponent>(GetDefaultSubobjectByName("SceneComp"));
	CapsuleComponent->bDrawOnlyIfSelected = true;
	CapsuleComponent->InitCapsuleSize(50.0f, 30.0f);
	CapsuleComponent->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	CapsuleComponent->bShouldCollideWhenPlacing = true;
	CapsuleComponent->SetShouldUpdatePhysicsVolume(false);
	CapsuleComponent->Mobility = EComponentMobility::Static;
	bCollideWhenPlacing = true;
	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
#endif
}

#if WITH_EDITOR
void AAuraEnemySpawnPoint::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	static const FName NAME_EnemyClass = GET_MEMBER_NAME_CHECKED(AAuraEnemySpawnPoint, EnemyClass);

	if (PropertyChangedEvent.MemberProperty)
	{
		if (PropertyChangedEvent.MemberProperty->GetFName() == NAME_EnemyClass)
		{
			if (EnemyClass != nullptr)
			{
				const AAuraEnemyCharacter* CDO = EnemyClass->GetDefaultObject<AAuraEnemyCharacter>();
				const UCapsuleComponent* EnemyCapsuleComponent = CDO->GetCapsuleComponent();
				CapsuleComponent->SetCapsuleHalfHeight(EnemyCapsuleComponent->GetScaledCapsuleHalfHeight());
				CapsuleComponent->SetCapsuleRadius(EnemyCapsuleComponent->GetScaledCapsuleRadius());
			}
		}
	}
}
#endif


void AAuraEnemySpawnPoint::SpawnEnemy() const
{
	if (AAuraEnemyCharacter* Enemy = GetWorld()->SpawnActorDeferred<AAuraEnemyCharacter>(
		EnemyClass, FTransform(), nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn))
	{
		Enemy->SetCharacterLevel(EnemyLevel);

		Enemy->FinishSpawning(GetActorTransform());
		
		if (Enemy->Controller == nullptr )
		{
			Enemy->SpawnDefaultController();
		}
	}
}
