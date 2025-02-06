// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AuraEffectActor.generated.h"

UCLASS()
class AURA_API AAuraEffectActor : public AActor
{
    GENERATED_BODY()

protected:

    UPROPERTY(EditAnywhere, Category = "Effects", BlueprintReadOnly)
    TSubclassOf<UGameplayEffect> InstantGameplayEffectClass;

    UPROPERTY(EditAnywhere, Category = "Effects", BlueprintReadOnly)
    float InstantGameplayEffectLevel = 1.f;

public:

    AAuraEffectActor();


protected:
    virtual void BeginPlay() override;

    UFUNCTION(BlueprintCallable)
    bool ApplyEffectToTarget(AActor* TargetActor);
};
