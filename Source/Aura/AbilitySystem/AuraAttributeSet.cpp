// Copyright 4sandwiches


#include "AbilitySystem/AuraAttributeSet.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraAbilitySystemTypes.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffectTypes.h"
#include "GameplayEffectExtension.h"
#include "AuraGameplayTags.h"
#include "Characters/CombatInterface.h"
#include "Characters/AuraCharacterBase.h"
#include "Game/AuraBlueprintFunctionLibrary.h"
#include "Player/AuraPlayerController.h"
#include "GameplayEffectComponents/TargetTagsGameplayEffectComponent.h"
#include "Logs.h"

#include "DebugHelper.h"

#pragma region FEffectProperties

class FEffectProperties
{
    mutable UAbilitySystemComponent* SourceASC = nullptr;

    mutable AActor* SourceAvatarActor = nullptr;
    mutable AActor* TargetAvatarActor = nullptr;

    mutable AController* SourceController = nullptr;
    mutable AController* TargetController = nullptr;

    mutable ACharacter* SourceCharacter = nullptr;
    mutable ACharacter* TargetCharacter = nullptr;

    const FGameplayEffectModCallbackData& Data;

public:

    FEffectProperties(const FGameplayEffectModCallbackData& InData)
        : Data(InData)
    {
    }

    UWorld* GetWorld() const
    {
        return Data.Target.GetWorld();
    }

    FGameplayEffectContextHandle GetEffectContextHandle() const
    {
        return Data.EffectSpec.GetContext();
    }

    UAbilitySystemComponent* GetSourceAbilitySystemComponent() const
    {
        if (SourceASC == nullptr)
        {
            SourceASC = GetEffectContextHandle().GetOriginalInstigatorAbilitySystemComponent();
        }
        return SourceASC;
    }

    AActor* GetSourceAvatarActor() const
    {
        if (SourceAvatarActor == nullptr)
        {
            auto ASC = GetSourceAbilitySystemComponent();
            if (IsValid(ASC))
            {
                SourceAvatarActor = ASC->GetAvatarActor();
            }
        }
        return SourceAvatarActor;
    }

    ACharacter* GetSourceCharacter() const
    {
        if (SourceCharacter == nullptr)
        {
            SourceCharacter = Cast<ACharacter>(GetSourceAvatarActor());
        }
        return SourceCharacter;
    }

    template<class T>
    T* GetSourceCharacter() const { return Cast<T>(GetSourceCharacter()); }

    bool IsTargetLocallyControlledPlayer() const
    {
        return GetEffectContextHandle().IsLocallyControlledPlayer();
    }

    AController* GetSourceController() const
    {
        if (SourceController == nullptr)
        {
            const auto ASC = GetSourceAbilitySystemComponent();
            if (IsValid(ASC))
            {
                if (ASC->AbilityActorInfo.IsValid() && ASC->AbilityActorInfo->PlayerController.IsValid())
                {
                    SourceController = ASC->AbilityActorInfo->PlayerController.Get();
                }
                else if (AActor* OwnerActor = ASC->GetOwner())
                {
                    AActor* TestActor = OwnerActor;
                    while (TestActor)
                    {
                        if (AController* CastPC = Cast<AController>(TestActor))
                        {
                            SourceController = CastPC;
                            break;
                        }

                        if (APawn* Pawn = Cast<APawn>(TestActor))
                        {
                            SourceController = Pawn->GetController();
                            break;
                        }

                        TestActor = TestActor->GetOwner();
                    }
                }
            }
        }
        return SourceController;
    }

    template<class T>
    T* GetSourceController() const { return Cast<T>(GetSourceController()); }

    UAbilitySystemComponent* GetTargetAbilitySystemComponent() const
    {
        return &Data.Target;
    }

    AActor* GetTargetAvatarActor() const
    {
        if (TargetAvatarActor == nullptr)
        {
            TargetAvatarActor = GetTargetAbilitySystemComponent()->GetAvatarActor();
        }
        return TargetAvatarActor;
    }

    ACharacter* GetTargetCharacter() const
    {
        if (TargetCharacter == nullptr)
        {
            TargetCharacter = Cast<ACharacter>(GetTargetAvatarActor());
        }
        return TargetCharacter;
    }

    template<class T>
    T* GetTargetCharacter() const { return Cast<T>(GetTargetCharacter()); }

    AController* GetTargetController() const
    {
        if (TargetController == nullptr)
        {
            const auto ASC = GetTargetAbilitySystemComponent();
            check(ASC != nullptr);
            if (ASC->AbilityActorInfo.IsValid() && ASC->AbilityActorInfo->PlayerController.IsValid())
            {
                TargetController = ASC->AbilityActorInfo->PlayerController.Get();
            }
            else if (AActor* OwnerActor = ASC->GetOwner())
            {
                AActor* TestActor = OwnerActor;
                while (TestActor)
                {
                    if (AController* CastPC = Cast<AController>(TestActor))
                    {
                        TargetController = CastPC;
                        break;
                    }

                    if (APawn* Pawn = Cast<APawn>(TestActor))
                    {
                        TargetController = Pawn->GetController();
                        break;
                    }

                    TestActor = TestActor->GetOwner();
                }
            }
        }
        return TargetController;
    }

    template<class T>
    T* GetTargetController() const { return Cast<T>(GetTargetController()); }

};
// FEffectProperties
#pragma endregion 

UAuraAttributeSet::UAuraAttributeSet()
{
}

void UAuraAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
    Super::PreAttributeChange(Attribute, NewValue);

}

void UAuraAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
    Super::PostGameplayEffectExecute(Data);

    FEffectProperties EffectProps(Data);

    if (EffectProps.GetTargetCharacter()->Implements<UCombatInterface>() && ICombatInterface::Execute_IsDead(EffectProps.GetTargetCharacter()))
    {
        return;
    }

    if (Data.EvaluatedData.Attribute == GetHealthAttribute())
    {
        const float NewHealth = FMath::Clamp(GetHealth(), 0.f, GetMaxHealth());
        SetHealth(NewHealth);
    }
    else if (Data.EvaluatedData.Attribute == GetManaAttribute())
    {
        const float NewMana = FMath::Clamp(GetMana(), 0.f, GetMaxMana());
        SetMana(NewMana);
    }
    else if (Data.EvaluatedData.Attribute == GetIncomingDamageAttribute())
    {
        HandleIncomingDamage(EffectProps);
    }
    else if (Data.EvaluatedData.Attribute == GetIncomingXPAttribute())
    {
        HandleIncomingXP(EffectProps);
    }
}

void UAuraAttributeSet::HandleIncomingDamage(const FEffectProperties& EffectProps)
{
    const float Damage = GetIncomingDamage();
    SetIncomingDamage(0.f);

    if (Damage > 0.f)
    {
        const float NewHealth = GetHealth() - Damage;
        SetHealth(FMath::Clamp(NewHealth, 0.f, GetMaxHealth()));

        const bool bCritHit = UAuraBlueprintFunctionLibrary::IsCriticalHit(EffectProps.GetEffectContextHandle());
        if (NewHealth <= 0.f)
        {
            if (ICombatInterface* Interface = Cast<ICombatInterface>(EffectProps.GetTargetAvatarActor()))
            {
                const FVector DeathImpulse = UAuraBlueprintFunctionLibrary::GetDeathImpulse(EffectProps.GetEffectContextHandle());
                Interface->Die(DeathImpulse * (bCritHit ? 2.f : 1.f));
            }
            SendXPEvent(EffectProps);
        }
        else
        {
            EffectProps.GetTargetAbilitySystemComponent()->TryActivateAbilitiesByTag(FGameplayTagContainer(AuraGameplayTags::Abilities_HitReact));
            if (ICombatInterface* Interface = Cast<ICombatInterface>(EffectProps.GetTargetAvatarActor()))
            {
                const FVector KnockBackImpulse = UAuraBlueprintFunctionLibrary::GetKnockBackImpulse(EffectProps.GetEffectContextHandle());
                Interface->KnockBack(KnockBackImpulse * (bCritHit ? 2.f : 1.f));
            }
        }

        const bool bBlocked = UAuraBlueprintFunctionLibrary::IsBlockedHit(EffectProps.GetEffectContextHandle());
        const bool bCriticalHit = UAuraBlueprintFunctionLibrary::IsCriticalHit(EffectProps.GetEffectContextHandle());
        ShowFloatingText(EffectProps, Damage, bBlocked, bCriticalHit);

        if (UAuraBlueprintFunctionLibrary::IsSuccessfulDebuff(EffectProps.GetEffectContextHandle()))
        {
            HandleDebuff(EffectProps);
        }
    }
}

void UAuraAttributeSet::HandleDebuff(const FEffectProperties& EffectProps)
{
    const auto [DebuffDamage, DebuffDuration, DebuffFrequency, DamageType] = UAuraBlueprintFunctionLibrary::GetDebuffParams(EffectProps.GetEffectContextHandle());

    // Create and apply Gameplay Effect dynamically
    
    FGameplayEffectContextHandle DebuffEffectContext = EffectProps.GetSourceAbilitySystemComponent()->MakeEffectContext();

    const FString DebuffName = FString::Printf(TEXT("DynamicDebuff_%s"), *DamageType.ToString());
    UGameplayEffect* DebuffEffect = NewObject<UGameplayEffect>(GetTransientPackage(), *DebuffName);

    DebuffEffect->DurationPolicy = EGameplayEffectDurationType::HasDuration;
    DebuffEffect->Period = DebuffFrequency;
    DebuffEffect->DurationMagnitude = FScalableFloat(DebuffDuration);

    FInheritedTagContainer GrantedTagsComp;
    GrantedTagsComp.Added.AddTag(FGameplayTagHelper::GetDebuffTagByDamageType(DamageType));
    DebuffEffect->AddComponent<UTargetTagsGameplayEffectComponent>().SetAndApplyTargetTagChanges(GrantedTagsComp);

    DebuffEffect->StackingType = EGameplayEffectStackingType::AggregateBySource;
    DebuffEffect->StackLimitCount = 1;

    FGameplayModifierInfo& ModInfo = DebuffEffect->Modifiers.Add_GetRef(FGameplayModifierInfo());
    ModInfo.ModifierMagnitude = FScalableFloat(DebuffDamage);
    ModInfo.ModifierOp = EGameplayModOp::Additive;
    ModInfo.Attribute = GetIncomingDamageAttribute();

    if (FGameplayEffectSpec* Spec = new FGameplayEffectSpec(DebuffEffect, DebuffEffectContext, 1.f))
    {
        const auto AuraContext = static_cast<FAuraGameplayEffectContext*>(DebuffEffectContext.Get());
        AuraContext->SetDamageType(DamageType);

        const FActiveGameplayEffectHandle ActiveEffectHandle = EffectProps.GetTargetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*Spec);

        // Add a Gameplay Cue. Because the debuff effect is a periodic,
        // we must control the added cue manually and remove it when the effect is removed
        const FGameplayTag GameplayCueTag = FGameplayTagHelper::GetDebuffCueTagByDamageType(DamageType);
        FGameplayCueParameters GameplayCueParams(DebuffEffectContext);
        GameplayCueParams.TargetAttachComponent = EffectProps.GetTargetCharacter()->GetMesh();
        GameplayCueParams.RawMagnitude = DebuffDamage;
        EffectProps.GetTargetAbilitySystemComponent()->AddGameplayCue(GameplayCueTag, GameplayCueParams);

        //UE_LOG(LogMyGame, Warning, TEXT("Adding Gameplay Cue: [%s]"), *GameplayCueTag.ToString());
        EffectProps.GetTargetAbilitySystemComponent()->OnGameplayEffectRemoved_InfoDelegate(ActiveEffectHandle)->AddLambda(
            [GameplayCueTag, TargetASC = EffectProps.GetTargetAbilitySystemComponent()](const FGameplayEffectRemovalInfo& )
            {
                // Note: we don't need to check TargetASC because it's one who calls this lambda.
                //   It's wierd, but I didn't find a way to extract the calling ASC from the arg.
                //   EffectContext.InstigatorASC - is SourceASC in terms of EffectProps
                
                //UE_LOG(LogMyGame, Warning, TEXT("Removing Gameplay Cue: [%s]"), *GameplayCueTag.ToString());
                TargetASC->RemoveGameplayCue(GameplayCueTag);
            });

    }
}

void UAuraAttributeSet::HandleIncomingXP(const FEffectProperties& EffectProps)
{
    const int32 XP = FMath::TruncToInt(GetIncomingXP());
    if (XP > 0)
    {
        SetIncomingXP(0.f);
        //Debug::Print(FString::Printf(TEXT("Incoming [%d] XP"), XP));

        if (EffectProps.GetSourceAvatarActor() && EffectProps.GetSourceAvatarActor()->Implements<UCombatInterface>())
        {
            ICombatInterface::Execute_AddXP(EffectProps.GetSourceAvatarActor(), XP);
        }
    }
}

void UAuraAttributeSet::ShowFloatingText(const FEffectProperties& EffectProps, const float Damage, bool bBlockedHit, bool bCriticalHit)
{
    if (EffectProps.GetSourceCharacter() != EffectProps.GetTargetCharacter())
    {
        if (const auto PC = EffectProps.GetSourceController<AAuraPlayerController>())
        {
            PC->ClientShowDamageFloatingNumber(EffectProps.GetTargetCharacter(), Damage, bBlockedHit, bCriticalHit);
        }
        if (const auto PC = EffectProps.GetTargetController<AAuraPlayerController>())
        {
            PC->ClientShowDamageFloatingNumber(EffectProps.GetTargetCharacter(), Damage, bBlockedHit, bCriticalHit);
        }
    }
}

void UAuraAttributeSet::SendXPEvent(const FEffectProperties& EffectProps)
{
    if (EffectProps.GetTargetAvatarActor() && EffectProps.GetTargetAvatarActor()->Implements<UCombatInterface>())
    {
        int32 XPReward = ICombatInterface::Execute_GetRewardXP(EffectProps.GetTargetAvatarActor());

        FGameplayEventData Payload;
        Payload.EventTag =AuraGameplayTags::Attributes_Meta_IncomingXP;
        Payload.EventMagnitude = XPReward;
        UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(EffectProps.GetSourceAvatarActor(),
            AuraGameplayTags::Attributes_Meta_IncomingXP, Payload);
    }
    
}

#pragma region Replication

void UAuraAttributeSet::OnRep_Strength(const FGameplayAttributeData& OldValue) const
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Strength, OldValue);
}

void UAuraAttributeSet::OnRep_Intelligence(const FGameplayAttributeData& OldValue) const
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Intelligence, OldValue);
}

void UAuraAttributeSet::OnRep_Resilience(const FGameplayAttributeData& OldValue) const
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Resilience, OldValue);
}

void UAuraAttributeSet::OnRep_Vigor(const FGameplayAttributeData& OldValue) const
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Vigor, OldValue);
}

void UAuraAttributeSet::OnRep_Health(const FGameplayAttributeData& OldValue) const
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Health, OldValue);
}

void UAuraAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldValue) const
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, MaxHealth, OldValue);
}

void UAuraAttributeSet::OnRep_Mana(const FGameplayAttributeData& OldValue) const
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Mana, OldValue);
}

void UAuraAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldValue) const
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, MaxMana, OldValue);
}

void UAuraAttributeSet::OnRep_Armor(const FGameplayAttributeData& OldValue) const
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Armor, OldValue);
}

void UAuraAttributeSet::OnRep_ArmorPenetration(const FGameplayAttributeData& OldValue) const
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, ArmorPenetration, OldValue);
}

void UAuraAttributeSet::OnRep_BlockChance(const FGameplayAttributeData& OldValue) const
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, BlockChance, OldValue);
}

void UAuraAttributeSet::OnRep_CritHitChance(const FGameplayAttributeData& OldValue) const
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, CritHitChance, OldValue);
}

void UAuraAttributeSet::OnRep_CritHitDamage(const FGameplayAttributeData& OldValue) const
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, CritHitDamage, OldValue);
}

void UAuraAttributeSet::OnRep_CritHitResistance(const FGameplayAttributeData& OldValue) const
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, CritHitResistance, OldValue);
}

void UAuraAttributeSet::OnRep_HealthRegen(const FGameplayAttributeData& OldValue) const
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, HealthRegen, OldValue);
}

void UAuraAttributeSet::OnRep_ManaRegen(const FGameplayAttributeData& OldValue) const
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, ManaRegen, OldValue);
}

void UAuraAttributeSet::OnRep_PhysicalResistance(const FGameplayAttributeData& OldValue) const
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, PhysicalResistance, OldValue);
}

void UAuraAttributeSet::OnRep_FireResistance(const FGameplayAttributeData& OldValue) const
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, FireResistance, OldValue);
}

void UAuraAttributeSet::OnRep_LightningResistance(const FGameplayAttributeData& OldValue) const
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, LightningResistance, OldValue);
}

void UAuraAttributeSet::OnRep_ArcaneResistance(const FGameplayAttributeData& OldValue) const
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, ArcaneResistance, OldValue);
}

void UAuraAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Strength, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Intelligence, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Resilience, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Vigor, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Health, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Mana, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Armor, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, ArmorPenetration, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, BlockChance, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, CritHitChance, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, CritHitDamage, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, CritHitResistance, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, ManaRegen, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, HealthRegen, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, PhysicalResistance, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, FireResistance, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, LightningResistance, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, ArcaneResistance, COND_None, REPNOTIFY_Always);
}

#pragma endregion
