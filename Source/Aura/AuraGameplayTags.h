// Copyright 4sandwiches

#pragma once

#include "NativeGameplayTags.h"

namespace AuraGameplayTags
{
    /**  Primary Attribute Tags **/

    AURA_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Primary_Strength);
    AURA_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Primary_Intelligence);
    AURA_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Primary_Resilience);
    AURA_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Primary_Vigor);


    /**  Secondary Attribute Tags **/

    AURA_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Secondary_Armor);
    AURA_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Secondary_ArmorPenetration);
    AURA_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Secondary_BlockChance);
    AURA_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Secondary_CritChance);
    AURA_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Secondary_CritDamage);
    AURA_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Secondary_CritResistance);
    AURA_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Secondary_HealthRegen);
    AURA_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Secondary_ManaRegen);
    AURA_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Secondary_MaxHealth);
    AURA_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Secondary_MaxMana);

    AURA_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Resistance);
    AURA_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Resistance_Physical);
    AURA_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Resistance_Fire);
    AURA_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Resistance_Lightning);
    AURA_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Resistance_Arcane);

    /**  Input Tags **/

    AURA_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_PrimaryAction);
    AURA_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_SecondaryAction);
    AURA_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Action1);
    AURA_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Action2);
    AURA_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Action3);
    AURA_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Action4);

    /**  Combat Tags **/

    AURA_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_BaseDamage);

    AURA_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Effects_HitReact);

    AURA_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Combat_Event_CritHit);
    AURA_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Combat_Event_Block);

    AURA_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Damage);
    AURA_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Damage_Physical);
    AURA_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Damage_Fire);
    AURA_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Damage_Lightning);
    AURA_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Damage_Arcane);
    
    AURA_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Abilities_Attack);
    AURA_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Abilities_Attack_Melee);
    AURA_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Abilities_Attack_Ranged);
    AURA_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Abilities_Attack_Spell);

    AURA_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Abilities_Summon);

    AURA_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(CombatSocket_Weapon);
    AURA_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(CombatSocket_LeftHand);
    AURA_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(CombatSocket_RightHand);

}

struct FGameplayTagHelper
{
    static FGameplayTagContainer RequestAllDamageTypeGameplayTags();
    static FGameplayTagContainer RequestAllResistanceGameplayTags();

    static FGameplayTag GetResistanceTagByDamageType(FGameplayTag DamageTypeTag);
    
private:
    static FGameplayTagHelper& Get();

    FGameplayTagHelper();

    TMap<FGameplayTag, FGameplayTag> ResistanceTagMap;
};