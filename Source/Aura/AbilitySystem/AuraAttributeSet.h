// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "AuraAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
 GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
 GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
 GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
 GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 *
 */
UCLASS()
class AURA_API UAuraAttributeSet : public UAttributeSet
{
    GENERATED_BODY()

public:

    UAuraAttributeSet();

    /*
    *  Primary Attributes
    */

    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Strength, Category = "Primary Attributes")
    FGameplayAttributeData Strength;
    ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Strength);

    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Intelligence, Category = "Primary Attributes")
    FGameplayAttributeData Intelligence;
    ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Intelligence);

    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Resilience, Category = "Primary Attributes")
    FGameplayAttributeData Resilience;
    ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Resilience);

    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Vigor, Category = "Primary Attributes")
    FGameplayAttributeData Vigor;
    ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Vigor);

    /*
    *  Vital Attributes
    */

    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health, Category = "Vital Attributes")
    FGameplayAttributeData Health;
    ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Health);

    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Mana, Category = "Vital Attributes")
    FGameplayAttributeData Mana;
    ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Mana);


    /*
    *  Secondary Attributes
    */

    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Armor, Category = "Secondary Attributes")
    FGameplayAttributeData Armor;
    ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Armor);

    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ArmorPenetration, Category = "Secondary Attributes")
    FGameplayAttributeData ArmorPenetration;
    ATTRIBUTE_ACCESSORS(UAuraAttributeSet, ArmorPenetration);

    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BlockChance, Category = "Secondary Attributes")
    FGameplayAttributeData BlockChance;
    ATTRIBUTE_ACCESSORS(UAuraAttributeSet, BlockChance);

    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CritHitChance, Category = "Secondary Attributes")
    FGameplayAttributeData CritHitChance;
    ATTRIBUTE_ACCESSORS(UAuraAttributeSet, CritHitChance);

    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CritHitDamage, Category = "Secondary Attributes")
    FGameplayAttributeData CritHitDamage;
    ATTRIBUTE_ACCESSORS(UAuraAttributeSet, CritHitDamage);

    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CritHitResistance, Category = "Secondary Attributes")
    FGameplayAttributeData CritHitResistance;
    ATTRIBUTE_ACCESSORS(UAuraAttributeSet, CritHitResistance);

    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HealthRegen, Category = "Secondary Attributes")
    FGameplayAttributeData HealthRegen;
    ATTRIBUTE_ACCESSORS(UAuraAttributeSet, HealthRegen);

    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ManaRegen, Category = "Secondary Attributes")
    FGameplayAttributeData ManaRegen;
    ATTRIBUTE_ACCESSORS(UAuraAttributeSet, ManaRegen);

    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth, Category = "Vital Attributes")
    FGameplayAttributeData MaxHealth;
    ATTRIBUTE_ACCESSORS(UAuraAttributeSet, MaxHealth);

    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxMana, Category = "Vital Attributes")
    FGameplayAttributeData MaxMana;
    ATTRIBUTE_ACCESSORS(UAuraAttributeSet, MaxMana);

    /*
    *  Meta Attributes
    */

    UPROPERTY(BlueprintReadOnly, Category = "Meta Attributes")
    FGameplayAttributeData IncomingDamage;
    ATTRIBUTE_ACCESSORS(UAuraAttributeSet, IncomingDamage);

public:

    void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
    void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;

    UFUNCTION()
    void OnRep_Strength(const FGameplayAttributeData& OldValue) const;

    UFUNCTION()
    void OnRep_Intelligence(const FGameplayAttributeData& OldValue) const;

    UFUNCTION()
    void OnRep_Resilience(const FGameplayAttributeData& OldValue) const;

    UFUNCTION()
    void OnRep_Vigor(const FGameplayAttributeData& OldValue) const;

    UFUNCTION()
    void OnRep_Health(const FGameplayAttributeData& OldValue) const;

    UFUNCTION()
    void OnRep_MaxHealth(const FGameplayAttributeData& OldValue) const;

    UFUNCTION()
    void OnRep_Mana(const FGameplayAttributeData& OldValue) const;

    UFUNCTION()
    void OnRep_MaxMana(const FGameplayAttributeData& OldValue) const;

    UFUNCTION()
    void OnRep_Armor(const FGameplayAttributeData& OldValue) const;

    UFUNCTION()
    void OnRep_ArmorPenetration(const FGameplayAttributeData& OldValue) const;

    UFUNCTION()
    void OnRep_BlockChance(const FGameplayAttributeData& OldValue) const;

    UFUNCTION()
    void OnRep_CritHitChance(const FGameplayAttributeData& OldValue) const;

    UFUNCTION()
    void OnRep_CritHitDamage(const FGameplayAttributeData& OldValue) const;

    UFUNCTION()
    void OnRep_CritHitResistance(const FGameplayAttributeData& OldValue) const;

    UFUNCTION()
    void OnRep_HealthRegen(const FGameplayAttributeData& OldValue) const;

    UFUNCTION()
    void OnRep_ManaRegen(const FGameplayAttributeData& OldValue) const;

    void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
