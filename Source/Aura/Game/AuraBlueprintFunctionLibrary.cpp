// Copyright 4sandwiches


#include "Game/AuraBlueprintFunctionLibrary.h"

#include "AbilitySystem/AuraAbilitySystemTypes.h"
#include "Algo/Accumulate.h"
#include "Algo/ForEach.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "UI/HUD/AuraHUD.h"
#include "Game/AuraGameModeBase.h"
#include "Characters/CombatInterface.h"

UOverlayWidgetController* UAuraBlueprintFunctionLibrary::GetOverlayWidgetController(const UObject* WorldContextObject)
{
    if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
    {
        checkf(PC->IsLocalController(), TEXT("The Player Controller must be a local player controller only."));

        if (AAuraHUD* AuraHUD = PC->GetHUD<AAuraHUD>())
        {
            return AuraHUD->GetOverlayWidgetController();
        }
    }

    return nullptr;
}

UAttributeMenuWidgetController* UAuraBlueprintFunctionLibrary::GetAttributeMenuWidgetController(const UObject* WorldContextObject)
{
    if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
    {
        checkf(PC->IsLocalController(), TEXT("The Player Controller must be a local player controller only."));

        if (AAuraHUD* AuraHUD = PC->GetHUD<AAuraHUD>())
        {
            return AuraHUD->GetAttributeMenuWidgetController();
        }
    }

    return nullptr;
}

UCharacterClassInfo* UAuraBlueprintFunctionLibrary::GetCharacterClassInfo(const UObject* WorldContextObject)
{
    if (auto GM = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject)))
    {
        return GM->CharacterClassInfo;
    }
    return nullptr;
}

int32 UAuraBlueprintFunctionLibrary::GetCharacterLevel(const AActor* Actor)
{
    if (const ICombatInterface* Interface = Cast<ICombatInterface>(Actor))
    {
        return Interface->GetCharacterLevel();
    }
    return 1;
}

bool UAuraBlueprintFunctionLibrary::IsBlockedHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
    // Note: we can cast to FAuraGameplayEffectContext because we set this in FAuraAbilitySystemGlobals which is set in DefaultGame.ini
    if (const FAuraGameplayEffectContext* EffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
    {
        return EffectContext->IsBlockedHit();
    }
    return false;
}

bool UAuraBlueprintFunctionLibrary::IsCriticalHit(const struct FGameplayEffectContextHandle& EffectContextHandle)
{
    // Note: we can cast to FAuraGameplayEffectContext because we set this in FAuraAbilitySystemGlobals which is set in DefaultGame.ini
    if (const FAuraGameplayEffectContext* EffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
    {
        return EffectContext->IsCriticalHit();
    }
    return false;
}

void UAuraBlueprintFunctionLibrary::SetIsBlockedHit(FGameplayEffectContextHandle& EffectContextHandle, bool bBlockedHit)
{
    // Note: we can cast to FAuraGameplayEffectContext because we set this in FAuraAbilitySystemGlobals which is set in DefaultGame.ini
    if (FAuraGameplayEffectContext* EffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
    {
        EffectContext->SetIsBlockedHit(bBlockedHit);
    }
}

void UAuraBlueprintFunctionLibrary::SetIsCriticalHit(FGameplayEffectContextHandle& EffectContextHandle, bool bCriticalHit)
{
    // Note: we can cast to FAuraGameplayEffectContext because we set this in FAuraAbilitySystemGlobals which is set in DefaultGame.ini
    if (FAuraGameplayEffectContext* EffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
    {
        EffectContext->SetIsCriticalHit(bCriticalHit);
    }
}

APawn* UAuraBlueprintFunctionLibrary::FindNearestAlivePawn(FVector Origin, const TArray<APawn*>& ActorsToCheck, float& Distance)
{
    APawn* NearestActor = nullptr;
    float DistanceFromNearestActor = Distance = TNumericLimits<float>::Max();

    for (APawn* ActorToCheck : ActorsToCheck)
    {
        if (ActorToCheck)
        {
            const float DistanceFromActorToCheck = (Origin - ActorToCheck->GetActorLocation()).SizeSquared();
            if (DistanceFromActorToCheck < DistanceFromNearestActor)
            {
                if (ActorToCheck->Implements<UCombatInterface>() && !ICombatInterface::Execute_IsDead(ActorToCheck))
                {
                    NearestActor = ActorToCheck;
                    DistanceFromNearestActor = DistanceFromActorToCheck;
                }
            }
        }
    }

    if (NearestActor)
    {
        Distance = FMath::Sqrt(DistanceFromNearestActor);
    }

    return NearestActor;
}

void UAuraBlueprintFunctionLibrary::GetAllLivePlayersInRadius(const UObject* WorldContextObject,
                                                              TArray<AActor*>& LivePlayers, const float Radius, const FVector& Origin, const TArray<AActor*>& IgnoreActors)
{
    FCollisionQueryParams SphereParams;

    SphereParams.AddIgnoredActors(IgnoreActors);

    // query scene to see what we hit
    TArray<FOverlapResult> Overlaps;
    if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
    {
        World->OverlapMultiByObjectType(Overlaps, Origin, FQuat::Identity,
            FCollisionObjectQueryParams(FCollisionObjectQueryParams::InitType::AllDynamicObjects),
            FCollisionShape::MakeSphere(Radius), SphereParams);

        LivePlayers.Reset(Overlaps.Num());
        for (const FOverlapResult& Overlap : Overlaps)
        {
            AActor* Actor = Overlap.GetActor();
            if (IsValid(Actor) && Actor->Implements<UCombatInterface>())
            {
                if (false == ICombatInterface::Execute_IsDead(Actor))
                {
                    LivePlayers.AddUnique(Actor);
                }
            }
        }
    }
}

bool UAuraBlueprintFunctionLibrary::AreFriendly(const AActor* A, const AActor* B)
{
    const FName PlayerTag = FName("Player");
    const FName EnemyTag = FName("Enemy");
    return (A->ActorHasTag(PlayerTag) && B->ActorHasTag(PlayerTag)) ||
        (A->ActorHasTag(EnemyTag) && B->ActorHasTag(EnemyTag)); 
}

int32 UAuraBlueprintFunctionLibrary::RandomIntWithWeights(TArray<float> Weights)
{
    int32 const NumElements = Weights.Num();
    if (NumElements <= 0)
    {
        return INDEX_NONE;
    }
    if (NumElements == 1)
    {
        return 0;/*first index*/
    }
    const float Sum = Algo::Accumulate(Weights, 0.f);
    if (Sum <= 0.f)
    {
        return 0;/*first index*/
    }
    Algo::ForEach(Weights, [Sum](float& X) { X /= Sum; });

    float R = FMath::FRand();
    int32 Index = 0;
    while (R > Weights[Index])
    {
        R -= Weights[Index];
        ++Index;
    }
    check(Index < NumElements);
    return Index;
}
