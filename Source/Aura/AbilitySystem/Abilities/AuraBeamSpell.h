// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/DamageGameplayAbility.h"
#include "Characters/CombatInterface.h"
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

	UPROPERTY(BlueprintReadWrite, Category = "Beam", BlueprintSetter=SetPrimaryTargetActor)
	TObjectPtr<AActor> PrimaryTargetActor;

	UPROPERTY(BlueprintReadWrite, Category = "Beam")
	TObjectPtr<APlayerController> OwningPlayerController;

	UPROPERTY(BlueprintReadOnly, Category = "Beam")
	TArray<TObjectPtr<AActor>> SecondaryTargets;

	UPROPERTY(BlueprintReadOnly)
	int32 MaxNumSecondaryTargets = 5;

public:

	UFUNCTION(BlueprintCallable)
	void StoreDataFromHitResult(const FHitResult& HitResult);

	UFUNCTION(BlueprintCallable)
	void DisableMovement() const;

	UFUNCTION(BlueprintCallable)
	void EnableMovement() const;

	UFUNCTION(BlueprintCallable)
	void SetPrimaryTargetActor(AActor* NewPrimaryTarget);

	UFUNCTION(BlueprintCallable)
	void SetSecondaryTargets(const TArray<AActor*>& NewSecondaryTargets);

	UFUNCTION(BlueprintCallable)
	virtual int32 GetNumSecondaryTargets(const int32 InLevel) const;

	//~ Begin of UGameplayAbility interface
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	//~ End of UGameplayAbility interface

protected:
	UFUNCTION(BlueprintImplementableEvent)
	void OnTargetDied(AActor* DeadActor);

private:

	void BindTargetCallback(const TScriptInterface<ICombatInterface>& CombatInterface);
	void UnbindTargetCallback(const TScriptInterface<ICombatInterface>& CombatInterface);
};
