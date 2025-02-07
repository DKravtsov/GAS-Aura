// Copyright 4sandwiches


#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffectTypes.h"
#include "GameplayEffectExtension.h"


class FEffectProperties
{
    UAbilitySystemComponent* SourceASC = nullptr;
    //UAbilitySystemComponent* TargetASC = nullptr;

    AActor* SourceAvatarActor = nullptr;
    AActor* TargetAvatarActor = nullptr;

    AController* SourceController = nullptr;
    AController* TargetController = nullptr;

    ACharacter* SourceCharacter = nullptr;
    ACharacter* TargetCharacter = nullptr;

    FGameplayEffectContextHandle EffectContextHandle;

    const FGameplayEffectModCallbackData& Data;

public:

    FEffectProperties(const FGameplayEffectModCallbackData& InData)
        : Data(InData)
    {
    }

    FGameplayEffectContextHandle GetEffectContextHandle()
    {
        if (!EffectContextHandle.IsValid())
        {
            EffectContextHandle = Data.EffectSpec.GetContext();
        }
        return EffectContextHandle;
    }

    UAbilitySystemComponent* GetSourceAbilitySystemComponent()
    {
        if (SourceASC == nullptr)
        {
            SourceASC = GetEffectContextHandle().GetOriginalInstigatorAbilitySystemComponent();
        }
        return SourceASC;
    }

    AActor* GetSourceAvatarActor()
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

    ACharacter* GetSourceCharacter()
    {
        if (SourceCharacter == nullptr)
        {
            SourceCharacter = Cast<ACharacter>(GetSourceAvatarActor());
        }
        return SourceCharacter;
    }

    template<class T>
    T* GetSourceCharacter() { return Cast<T>(GetSourceCharacter()); }

    bool IsTargetLocallyControlledPlayer()
    {
        return GetEffectContextHandle().IsLocallyControlledPlayer();
    }

    AController* GetSourceController()
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
    T* GetSourceController() { return Cast<T>(GetSourceController()); }

    UAbilitySystemComponent* GetTargetAbilitySystemComponent()
    {
        return &Data.Target;
    }

    AActor* GetTargetAvatarActor()
    {
        if (TargetAvatarActor == nullptr)
        {
            TargetAvatarActor = GetTargetAbilitySystemComponent()->GetAvatarActor();
        }
        return TargetAvatarActor;
    }

    ACharacter* GetTargetCharacter()
    {
        if (TargetCharacter == nullptr)
        {
            TargetCharacter = Cast<ACharacter>(GetTargetAvatarActor());
        }
        return TargetCharacter;
    }

    template<class T>
    T* GetTargetCharacter() { return Cast<T>(GetTargetCharacter()); }

    AController* GetTargetController()
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
    T* GetTargetController() { return Cast<T>(GetTargetController()); }

};

UAuraAttributeSet::UAuraAttributeSet()
{
    InitHealth(10.f);
    InitMaxHealth(100.f);
    InitMana(50.f);
    InitMaxMana(50);
}

void UAuraAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
    Super::PreAttributeChange(Attribute, NewValue);

    //if (Attribute == GetHealthAttribute())
    //{
    //    NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
    //}
    //else if (Attribute == GetManaAttribute())
    //{
    //    NewValue = FMath::Clamp(NewValue, 0.f, GetMaxMana());
    //}
}

void UAuraAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
    Super::PostGameplayEffectExecute(Data);

    FEffectProperties EffectProps(Data);

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

void UAuraAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Health, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Mana, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);
}
