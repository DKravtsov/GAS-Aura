// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/DamageGameplayAbility.h"
#include "AuraBeamSpell.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraBeamSpell : public UDamageGameplayAbility
{
	GENERATED_BODY()
public:

protected:

	UPROPERTY(BlueprintReadWrite, Category = "Beam")
	FVector CursorHitLocation;

	UPROPERTY(BlueprintReadWrite, Category = "Beam")
	TObjectPtr<AActor> CursorHitActor;

	UPROPERTY(BlueprintReadWrite, Category = "Beam")
	TObjectPtr<APlayerController> OwningPlayerController;

public:

	UFUNCTION(BlueprintCallable)
	void StoreDataFromHitResult(const FHitResult& HitResult);

	UFUNCTION(BlueprintCallable)
	void DisableMovement() const;

	UFUNCTION(BlueprintCallable)
	void EnableMovement() const;

    virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
};
