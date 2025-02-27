// Copyright 4sandwiches


#include "AbilitySystem/Abilities/AuraBeamSpell.h"

#include "Characters/CombatInterface.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

void UAuraBeamSpell::StoreDataFromHitResult(const FHitResult& HitResult)
{
	if (HitResult.bBlockingHit)
	{
		CursorHitLocation = HitResult.ImpactPoint;
		SetPrimaryTargetActor(HitResult.GetActor());
	}
	else
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
	}
}

void UAuraBeamSpell::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);

}

void UAuraBeamSpell::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);

	if (ActorInfo)
	{
		OwningPlayerController = ActorInfo->PlayerController.Get();
	}
}

void UAuraBeamSpell::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	// Clear callbacks
	SetPrimaryTargetActor(nullptr);
	for (const auto& Target : SecondaryTargets)
	{
		UnbindTargetCallback(Target);
	}
}

void UAuraBeamSpell::DisableMovement() const
{
	if (const ACharacter* AvatarCharacter = Cast<ACharacter>(GetAvatarActorFromActorInfo()))
	{
		AvatarCharacter->GetCharacterMovement()->DisableMovement();
	}
}

void UAuraBeamSpell::EnableMovement() const
{
	if (const ACharacter* AvatarCharacter = Cast<ACharacter>(GetAvatarActorFromActorInfo()))
	{
		AvatarCharacter->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	}
}

int32 UAuraBeamSpell::GetNumSecondaryTargets() const
{
	return FMath::Min(GetAbilityLevel() - 1, MaxNumSecondaryTargets);
}

void UAuraBeamSpell::SetPrimaryTargetActor(AActor* NewPrimaryTarget)
{
	if (NewPrimaryTarget == PrimaryTargetActor)
	{
		return;
	}
	// Just in case, because we can change the primary target during ability execution
	UnbindTargetCallback(PrimaryTargetActor);
	PrimaryTargetActor = NewPrimaryTarget;
	BindTargetCallback(PrimaryTargetActor);
}

void UAuraBeamSpell::SetSecondaryTargets(const TArray<AActor*>& NewSecondaryTargets)
{
	//Note: We don't need to unbind callbacks for secondary targets because we unlikely will change them during
	//	the ability is processing. And we clear them up when the ability ends.
	SecondaryTargets = NewSecondaryTargets;

	for (auto Target : SecondaryTargets)
	{
		BindTargetCallback(Target);
	}
}

void UAuraBeamSpell::BindTargetCallback(const TScriptInterface<ICombatInterface>& CombatInterface)
{
	if (auto CI = CombatInterface.GetInterface())
	{
		if (!CI->GetOnDeathDelegate().IsAlreadyBound(this, &UAuraBeamSpell::OnTargetDied))
		{
			CI->GetOnDeathDelegate().AddDynamic(this, &UAuraBeamSpell::OnTargetDied);
		}
	}
}

void UAuraBeamSpell::UnbindTargetCallback(const TScriptInterface<ICombatInterface>& CombatInterface)
{
	if (ICombatInterface* CI = Cast<ICombatInterface>(PrimaryTargetActor))
	{
		CI->GetOnDeathDelegate().RemoveDynamic(this, &UAuraBeamSpell::OnTargetDied);
	}
}

