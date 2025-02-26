// Copyright 4sandwiches


#include "AbilitySystem/Abilities/AuraBeamSpell.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

void UAuraBeamSpell::StoreDataFromHitResult(const FHitResult& HitResult)
{
	if (HitResult.bBlockingHit)
	{
		CursorHitLocation = HitResult.ImpactPoint;
		CursorHitActor = HitResult.GetActor();
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
