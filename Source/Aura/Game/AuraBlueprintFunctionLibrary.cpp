// Copyright 4sandwiches


#include "Game/AuraBlueprintFunctionLibrary.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AuraGameInstance.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemTypes.h"
#include "AbilitySystem/Data/AbilityInfoDataAsset.h"
#include "Algo/Accumulate.h"
#include "Algo/ForEach.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "UI/HUD/AuraHUD.h"
#include "Game/AuraGameModeBase.h"
#include "Characters/CombatInterface.h"
#include "Kismet/KismetMathLibrary.h"

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

class USpellMenuWidgetController* UAuraBlueprintFunctionLibrary::GetSpellMenuWidgetController(const UObject* WorldContextObject)
{
    if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
    {
        checkf(PC->IsLocalController(), TEXT("The Player Controller must be a local player controller only."));

        if (AAuraHUD* AuraHUD = PC->GetHUD<AAuraHUD>())
        {
            return AuraHUD->GetSpellMenuWidgetController();
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

 UAbilityInfoDataAsset* UAuraBlueprintFunctionLibrary::GetAbilityInfo(const UObject* WorldContextObject)
{
    if (const auto GameInstance = Cast<UAuraGameInstance>(UGameplayStatics::GetGameInstance(WorldContextObject)))
    {
        return GameInstance->AbilityInfo;
    }
    return nullptr;
}

int32 UAuraBlueprintFunctionLibrary::GetCharacterLevel(const AActor* Actor)
{
    if (ensureAlways(Actor) && Actor->Implements<UCombatInterface>())
    {
        return ICombatInterface::Execute_BP_GetCharacterLevel(Actor);
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

bool UAuraBlueprintFunctionLibrary::IsSuccessfulDebuff(const FGameplayEffectContextHandle& EffectContextHandle)
{
    // Note: we can cast to FAuraGameplayEffectContext because we set this in FAuraAbilitySystemGlobals which is set in DefaultGame.ini
    if (const FAuraGameplayEffectContext* EffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
    {
        return EffectContext->IsSuccessfulDebuff();
    }
    return false;
}

UAuraBlueprintFunctionLibrary::FDebuffParams UAuraBlueprintFunctionLibrary::GetDebuffParams(const FGameplayEffectContextHandle& EffectContextHandle)
{
    // Note: we can cast to FAuraGameplayEffectContext because we set this in FAuraAbilitySystemGlobals which is set in DefaultGame.ini
    if (const FAuraGameplayEffectContext* EffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
    {
        
        return FDebuffParams(EffectContext->GetDebuffDamage(), EffectContext->GetDebuffDuration(),
            EffectContext->GetDebuffFrequency(), EffectContext->GetDamageType());
    }
    return FDebuffParams();
}

float UAuraBlueprintFunctionLibrary::GetDebuffDamage(const FGameplayEffectContextHandle& EffectContextHandle)
{
    // Note: we can cast to FAuraGameplayEffectContext because we set this in FAuraAbilitySystemGlobals which is set in DefaultGame.ini
    if (const FAuraGameplayEffectContext* EffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
    {
        return EffectContext->GetDebuffDamage();
    }
    return 0.f;
}

float UAuraBlueprintFunctionLibrary::GetDebuffDuration(const FGameplayEffectContextHandle& EffectContextHandle)
{
    // Note: we can cast to FAuraGameplayEffectContext because we set this in FAuraAbilitySystemGlobals which is set in DefaultGame.ini
    if (const FAuraGameplayEffectContext* EffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
    {
        return EffectContext->GetDebuffDuration();
    }
    return 0.f;
}

float UAuraBlueprintFunctionLibrary::GetDebuffFrequency(const FGameplayEffectContextHandle& EffectContextHandle)
{
    // Note: we can cast to FAuraGameplayEffectContext because we set this in FAuraAbilitySystemGlobals which is set in DefaultGame.ini
    if (const FAuraGameplayEffectContext* EffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
    {
        return EffectContext->GetDebuffFrequency();
    }
    return 0.f;
}

FGameplayTag UAuraBlueprintFunctionLibrary::GetDamageTypeFromEffectContext(const FGameplayEffectContextHandle& EffectContextHandle)
{
    // Note: we can cast to FAuraGameplayEffectContext because we set this in FAuraAbilitySystemGlobals which is set in DefaultGame.ini
    if (const FAuraGameplayEffectContext* EffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
    {
        return EffectContext->GetDamageType();
    }
    return FGameplayTag();
}

FVector UAuraBlueprintFunctionLibrary::GetDeathImpulse(const FGameplayEffectContextHandle& EffectContextHandle)
{
    // Note: we can cast to FAuraGameplayEffectContext because we set this in FAuraAbilitySystemGlobals which is set in DefaultGame.ini
    if (const FAuraGameplayEffectContext* EffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
    {
        return EffectContext->GetDeathImpulse();
    }
    return FVector();
}

FVector UAuraBlueprintFunctionLibrary::GetKnockBackImpulse(const FGameplayEffectContextHandle& EffectContextHandle)
{
    // Note: we can cast to FAuraGameplayEffectContext because we set this in FAuraAbilitySystemGlobals which is set in DefaultGame.ini
    if (const FAuraGameplayEffectContext* EffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
    {
        return EffectContext->GetKnockBackImpulse();
    }
    return FVector();
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

void UAuraBlueprintFunctionLibrary::SetIsSuccessfulDebuff(FGameplayEffectContextHandle& EffectContextHandle, bool bDebuff)
{
    // Note: we can cast to FAuraGameplayEffectContext because we set this in FAuraAbilitySystemGlobals which is set in DefaultGame.ini
    if (FAuraGameplayEffectContext* EffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
    {
        EffectContext->SetIsSuccessfulDebuff(bDebuff);
    }
}

void UAuraBlueprintFunctionLibrary::SetupDebuffParams(FGameplayEffectContextHandle& EffectContextHandle,
    float InDamage, float InDuration, float InFrequency, const FGameplayTag& InDamageType)
{
    // Note: we can cast to FAuraGameplayEffectContext because we set this in FAuraAbilitySystemGlobals which is set in DefaultGame.ini
    if (FAuraGameplayEffectContext* EffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
    {
        EffectContext->SetIsSuccessfulDebuff(true);
        EffectContext->SetDamageType(InDamageType);
        EffectContext->SetupDebuff(InDamage, InDuration, InFrequency);
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

FGameplayEffectContextHandle UAuraBlueprintFunctionLibrary::ApplyDamageEffect(const FDamageEffectParams& Params)
{
    FGameplayEffectContextHandle EffectContextHandle = Params.SourceAbilitySystemComponent->MakeEffectContext();
    FGameplayEffectSpecHandle DamageEffectSpecHandle = Params.SourceAbilitySystemComponent->MakeOutgoingSpec(Params.DamageEffectClass, Params.AbilityLevel, EffectContextHandle);

    UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(DamageEffectSpecHandle, Params.DamageType, Params.BaseDamage);

    UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(DamageEffectSpecHandle, AuraGameplayTags::Debuff_SetByCaller_Chance, Params.DebuffChance);
    UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(DamageEffectSpecHandle, AuraGameplayTags::Debuff_SetByCaller_Damage, Params.DebuffDamage);
    UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(DamageEffectSpecHandle, AuraGameplayTags::Debuff_SetByCaller_Duration, Params.DebuffDuration);
    UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(DamageEffectSpecHandle, AuraGameplayTags::Debuff_SetByCaller_Frequency, Params.DebuffFrequency);

    if (Params.HasHitResult())
    {
        DamageEffectSpecHandle.Data->GetContext().AddHitResult(*Params.GetHitResult());
    }
    // Note: we can cast to FAuraGameplayEffectContext because we set this in FAuraAbilitySystemGlobals which is set in DefaultGame.ini
    if (FAuraGameplayEffectContext* EffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
    {
        EffectContext->SetDeathImpulse(Params.DeathImpulse);
        EffectContext->SetKnockBackImpulse(Params.KnockBackImpulse);
    }
    
    check(Params.TargetAbilitySystemComponent);
    Params.TargetAbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*DamageEffectSpecHandle.Data);

    return EffectContextHandle;
    
}

// TArray<FGameplayEffectContextHandle> UAuraBlueprintFunctionLibrary::ApplyRadialDamageEffect(const FDamageEffectParams& Params,
//         const FVector& Origin, float DamageInnerRadius, float DamageOuterRadius, float DamageFalloff, const TArray<AActor*>& IgnoreActors)
// {
//     // bind callbacks
//     AActor* SourceAvatar = Params.SourceAbilitySystemComponent->GetAvatarActor();
//     if (!SourceAvatar->HasAuthority())
//         return {};
//     
//     TArray<AActor*> Players;
//     TArray<FGameplayEffectContextHandle> ReturnHandles;
//     GetAllLivePlayersInRadius(SourceAvatar, Players, DamageOuterRadius, Origin, IgnoreActors);
//     for (AActor* Player : Players)
//     {
//         if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(Player))
//         {
//             CombatInterface->GetActorTakeDamageDelegate().BindLambda(
//                 [&ReturnHandles](AActor* Actor, const float Damage, const FDamageEffectParams& OriginalParams)
//                 {
//                     auto RadialDamageParams = OriginalParams;
//                     ScaleDamageEffectParams(RadialDamageParams, Damage / OriginalParams.BaseDamage);
//                     ReturnHandles.Add(ApplyDamageEffect(RadialDamageParams));
//                 }, Params);
//         }
//     }
//     
//     // apply damage
//
//     UGameplayStatics::ApplyRadialDamageWithFalloff(SourceAvatar, Params.BaseDamage, 0.f, Origin,
//         DamageInnerRadius, DamageOuterRadius, DamageFalloff, nullptr, IgnoreActors, SourceAvatar,
//         Params.SourceAbilitySystemComponent->AbilityActorInfo->PlayerController.Get());
//
//     // unbind callbacks
//     for (AActor* Player : Players)
//     {
//         if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(Player))
//         {
//             CombatInterface->GetActorTakeDamageDelegate().Unbind();
//         }
//     }
//
//     return ReturnHandles;
// }

// void UAuraBlueprintFunctionLibrary::AddKnockBackParams(const AActor* TargetActor, const FVector& Origin, FDamageEffectParams& Params)
// {
//     FRotator LookARotator = UKismetMathLibrary::FindLookAtRotation(Origin, TargetActor->GetActorLocation());
//     // if (bKnockBackPitchOverride)
//     // {
//     //     LookARotator.Pitch = KnockBackPitch;
//     // }
//     const FVector TargetDirection = LookARotator.Vector();
//     Params.DeathImpulse = TargetDirection * Params.DeathImpulseMagnitude;
//     Params.KnockBackImpulse = TargetDirection * Params.KnockBackImpulseMagnitude;
// }

void UAuraBlueprintFunctionLibrary::ScaleDamageEffectParams(FDamageEffectParams& Params, const float Scale)
{
    Params.BaseDamage *= Scale;
    Params.DeathImpulseMagnitude *= Scale;
    Params.KnockBackImpulseMagnitude *= Scale;
    Params.DeathImpulse *= Scale;
    Params.KnockBackImpulse *= Scale;
}

TArray<FVector> UAuraBlueprintFunctionLibrary::GetUniformSpreadOfDirections(const FVector& Forward, const float SpreadAngle, const int32 NumDirections)
{
    TArray<FVector> Result;
    Result.Reserve(FMath::Max(NumDirections, 1));
    if (NumDirections > 1)
    {
        const FVector LeftOfSpread = Forward.RotateAngleAxis(-SpreadAngle/2.f, FVector::UpVector);
        const float DeltaSpread = SpreadAngle / (NumDirections - 1);
        for (int32 Index = 0; Index < NumDirections; ++Index)
        {
            const FVector Direction = LeftOfSpread.RotateAngleAxis(DeltaSpread * Index, FVector::UpVector);
            Result.Add(Direction);
        }
    }
    else
    {
        Result.Add(Forward);
    }
    return Result;
}

float UAuraBlueprintFunctionLibrary::FRandInRange(const FVector2D& MinMax)
{
    return FMath::FRandRange(MinMax.X, MinMax.Y);
}

float UAuraBlueprintFunctionLibrary::RandInRange(const FIntVector2& MinMax)
{
    return FMath::RandRange(MinMax.X, MinMax.Y);
}

bool UAuraBlueprintFunctionLibrary::DoesActorHaveAllGameplayTags(AActor* Actor, const FGameplayTagContainer& TagContainer)
{
    if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Actor))
    {
        return ASC->HasAllMatchingGameplayTags(TagContainer);
    }
    if (!IsValid(Actor))
    {
        return false;
    }
    if (IGameplayTagAssetInterface* Interface = Cast<IGameplayTagAssetInterface>(Actor))
    {
        return Interface->HasAllMatchingGameplayTags(TagContainer);
    }
    return false;
}

bool UAuraBlueprintFunctionLibrary::DoesActorHaveAnyGameplayTags(AActor* Actor, const FGameplayTagContainer& TagContainer)
{
    if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Actor))
    {
        return ASC->HasAnyMatchingGameplayTags(TagContainer);
    }
    if (!IsValid(Actor))
    {
        return false;
    }
    if (IGameplayTagAssetInterface* Interface = Cast<IGameplayTagAssetInterface>(Actor))
    {
        return Interface->HasAnyMatchingGameplayTags(TagContainer);
    }
    return false;
}

bool UAuraBlueprintFunctionLibrary::DoesActorHaveGameplayTag(AActor* Actor, FGameplayTag Tag)
{
    if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Actor))
    {
        return ASC->HasMatchingGameplayTag(Tag);
    }
    if (!IsValid(Actor))
    {
        return false;
    }
    if (IGameplayTagAssetInterface* Interface = Cast<IGameplayTagAssetInterface>(Actor))
    {
        return Interface->HasMatchingGameplayTag(Tag);
    }
    return false;
}

void UAuraBlueprintFunctionLibrary::AddGameplayTagToActor(AActor* Actor, FGameplayTag Tag)
{
    if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Actor))
    {
        if (!ASC->HasMatchingGameplayTag(Tag))
        {
            ASC->AddLooseGameplayTag(Tag);
        }
    }
}

void UAuraBlueprintFunctionLibrary::RemoveGameplayTagFromActor(AActor* Actor, FGameplayTag Tag)
{
    if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Actor))
    {
        if (ASC->HasMatchingGameplayTag(Tag))
        {
            ASC->RemoveLooseGameplayTag(Tag);
        }
    }
}

void UAuraBlueprintFunctionLibrary::AddGameplayTagToActor_Replicated(AActor* Actor, FGameplayTag Tag)
{
    if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Actor))
    {
        if (!ASC->HasMatchingGameplayTag(Tag))
        {
            ASC->AddLooseGameplayTag(Tag);
            ASC->AddReplicatedLooseGameplayTag(Tag);
        }
    }
}

void UAuraBlueprintFunctionLibrary::RemoveGameplayTagFromActor_Replicated(AActor* Actor, FGameplayTag Tag)
{
    if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Actor))
    {
        if (ASC->HasMatchingGameplayTag(Tag))
        {
            ASC->RemoveLooseGameplayTag(Tag);
            ASC->RemoveReplicatedLooseGameplayTag(Tag);
        }
    }
}
