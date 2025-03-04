// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "Engine/TargetPoint.h"
#include "AuraEnemySpawnPoint.generated.h"

enum class EAuraCharacterClass : uint8;
/**
 * 
 */
UCLASS()
class AURA_API AAuraEnemySpawnPoint : public ATargetPoint
{
	GENERATED_BODY()

#if WITH_EDITORONLY_DATA
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	TObjectPtr<class UCapsuleComponent> CapsuleComponent;
#endif
	
public:

	AAuraEnemySpawnPoint(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="EnemySpawn")
	TSubclassOf<class AAuraEnemyCharacter> EnemyClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="EnemySpawn")
	int32 EnemyLevel = 1;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
public:

	UFUNCTION(BlueprintCallable)
	void SpawnEnemy() const;
};
