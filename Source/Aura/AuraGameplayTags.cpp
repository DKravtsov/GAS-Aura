// Copyright 4sandwiches

#include "AuraGameplayTags.h"

namespace AuraGameplayTags
{
    /**  Primary Attribute Tags **/

    UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Primary_Strength, "Attributes.Primary.Strength", "");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Primary_Intelligence, "Attributes.Primary.Intelligence", "");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Primary_Resilience, "Attributes.Primary.Resilience", "");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Primary_Vigor, "Attributes.Primary.Vigor", "");


    /**  Secondary Attribute Tags **/

    UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Secondary_Armor, "Attributes.Secondary.Armor", "");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Secondary_ArmorPenetration, "Attributes.Secondary.ArmorPenetration", "");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Secondary_BlockChance, "Attributes.Secondary.BlockChance", "");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Secondary_CritChance, "Attributes.Secondary.CritChance", "");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Secondary_CritDamage, "Attributes.Secondary.CritDamage", "");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Secondary_CritResistance, "Attributes.Secondary.CritResistance", "");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Secondary_HealthRegen, "Attributes.Secondary.HealthRegen", "");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Secondary_ManaRegen, "Attributes.Secondary.ManaRegen", "");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Secondary_MaxHealth, "Attributes.Secondary.MaxHealth", "");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Secondary_MaxMana, "Attributes.Secondary.MaxMana", "");

    UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Resistance, "Attributes.Resistance", "Parent Tag for all resistances");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Resistance_Physical, "Attributes.Resistance.Physical", "");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Resistance_Fire, "Attributes.Resistance.Fire", "");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Resistance_Lightning, "Attributes.Resistance.Lightning", "");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Resistance_Arcane, "Attributes.Resistance.Arcane", "");

    /**  Secondary Attribute Tags **/

    UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Meta_IncomingXP, "Attributes.Meta.IncomingXP", "");

    /**  Input Tags **/

    UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag, "InputTag", "Parent tag for all input tags");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_PrimaryAction, "InputTag.PrimaryAction", "LMB");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_SecondaryAction, "InputTag.SecondaryAction", "RMB");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_Action1, "InputTag.Action1", "1");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_Action2, "InputTag.Action2", "2");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_Action3, "InputTag.Action3", "3");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_Action4, "InputTag.Action4", "4");

    /**  Combat Tags **/

    UE_DEFINE_GAMEPLAY_TAG_COMMENT(Damage, "Damage", "Parent tag for all damage types");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(Damage_Physical, "Damage.Physical", "Physical damage type");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(Damage_Fire, "Damage.Fire", "Fire damage type");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(Damage_Lightning, "Damage.Lightning", "Lightning damage type");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(Damage_Arcane, "Damage.Arcane", "Arcane damage type");

    UE_DEFINE_GAMEPLAY_TAG_COMMENT(Debuff, "Debuff", "");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(Debuff_Type_Physical, "Debuff.Type.Physical", "");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(Debuff_Type_Burn, "Debuff.Type.Burn", "");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(Debuff_Type_Stun, "Debuff.Type.Stun", "");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(Debuff_Type_Arcane, "Debuff.Type.Arcane", "");

    UE_DEFINE_GAMEPLAY_TAG_COMMENT(Debuff_SetByCaller_Chance, "Debuff.SetByCaller.Chance", "");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(Debuff_SetByCaller_Damage, "Debuff.SetByCaller.Damage", "");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(Debuff_SetByCaller_Duration, "Debuff.SetByCaller.Duration", "");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(Debuff_SetByCaller_Frequency, "Debuff.SetByCaller.Frequency", "");

    UE_DEFINE_GAMEPLAY_TAG_COMMENT(Abilities, "Abilities", "Parent tag for all gameplay abilities");

    UE_DEFINE_GAMEPLAY_TAG_COMMENT(Abilities_None, "Abilities.None", "?");
    
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(Abilities_Attack, "Abilities.Attack", "Parent tag for all attacking abilities");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(Abilities_Attack_Melee, "Abilities.Attack.Melee", "Melee attack");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(Abilities_Attack_Ranged, "Abilities.Attack.Ranged", "Ranged attack");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(Abilities_Attack_Spell, "Abilities.Attack.Spell", "Magic spell cast");

    UE_DEFINE_GAMEPLAY_TAG_COMMENT(Abilities_Summon, "Abilities.Summon", "Ability to summon minions");

    UE_DEFINE_GAMEPLAY_TAG_COMMENT(Abilities_HitReact, "Abilities.HitReact", "");
    
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(Abilities_Status, "Abilities.Status", "");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(Abilities_Status_Locked, "Abilities.Status.Locked", "");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(Abilities_Status_Eligible, "Abilities.Status.Eligible", "");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(Abilities_Status_Unlocked, "Abilities.Status.Unlocked", "");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(Abilities_Status_Equipped, "Abilities.Status.Equipped", "");

    UE_DEFINE_GAMEPLAY_TAG_COMMENT(Abilities_Type, "Abilities.Type", "");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(Abilities_Type_None, "Abilities.Type.None", "");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(Abilities_Type_Offensive, "Abilities.Type.Offensive", "");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(Abilities_Type_Passive, "Abilities.Type.Passive", "");

    UE_DEFINE_GAMEPLAY_TAG_COMMENT(CombatSocket_Weapon, "CombatSocket.Weapon", "Get socket location from the equipped weapon");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(CombatSocket_LeftHand, "CombatSocket.LeftHand", "get socket location from the character's mesh left hand");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(CombatSocket_RightHand, "CombatSocket.RightHand", "get socket location from the character's mesh right hand");

    UE_DEFINE_GAMEPLAY_TAG_COMMENT(Cooldown, "Cooldown", "Parent tag for all cooldown tags");

    /**  Gameplay Cues  **/
    
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayCue, "GameplayCue", "");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayCue_Debuff, "GameplayCue.Debuff", "");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayCue_Debuff_Burn, "GameplayCue.Debuff.Burn", "");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayCue_Debuff_Shock, "GameplayCue.Debuff.Shock", "");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayCue_Debuff_Arcane, "GameplayCue.Debuff.Arcane", "");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayCue_Debuff_Physical, "GameplayCue.Debuff.Physical", "");
    
}

FGameplayTagContainer FGameplayTagHelper::RequestAllDamageTypeGameplayTags()
{
    return UGameplayTagsManager::Get().RequestGameplayTagChildren(AuraGameplayTags::Damage);
}

FGameplayTagContainer FGameplayTagHelper::RequestAllResistanceGameplayTags()
{
    return UGameplayTagsManager::Get().RequestGameplayTagChildren(AuraGameplayTags::Attributes_Resistance);
}

FGameplayTag FGameplayTagHelper::GetResistanceTagByDamageType(const FGameplayTag& DamageTypeTag)
{
    return Get().ResistanceTagMap.FindChecked(DamageTypeTag);
}

FGameplayTag FGameplayTagHelper::GetDebuffTagByDamageType(const FGameplayTag& DamageTypeTag)
{
    if (DamageTypeTag == AuraGameplayTags::Damage_Physical)
    {
        return AuraGameplayTags::Debuff_Type_Physical;
    }
    if (DamageTypeTag == AuraGameplayTags::Damage_Fire)
    {
        return AuraGameplayTags::Debuff_Type_Burn;
    }
    if (DamageTypeTag == AuraGameplayTags::Damage_Lightning)
    {
        return AuraGameplayTags::Debuff_Type_Stun;
    }
    if (DamageTypeTag == AuraGameplayTags::Damage_Arcane)
    {
        return AuraGameplayTags::Debuff_Type_Arcane;
    }
    return FGameplayTag();
}

FGameplayTag FGameplayTagHelper::GetDebuffCueTagByDamageType(const FGameplayTag& DamageTypeTag)
{
    if (DamageTypeTag == AuraGameplayTags::Damage_Physical)
    {
        return AuraGameplayTags::GameplayCue_Debuff_Physical;
    }
    if (DamageTypeTag == AuraGameplayTags::Damage_Fire)
    {
        return AuraGameplayTags::GameplayCue_Debuff_Burn;
    }
    if (DamageTypeTag == AuraGameplayTags::Damage_Lightning)
    {
        return AuraGameplayTags::GameplayCue_Debuff_Shock;
    }
    if (DamageTypeTag == AuraGameplayTags::Damage_Arcane)
    {
        return AuraGameplayTags::GameplayCue_Debuff_Arcane;
    }
    return FGameplayTag();
}

FGameplayTagHelper& FGameplayTagHelper::Get()
{
    static FGameplayTagHelper Helper;
    return Helper;
}

FGameplayTagHelper::FGameplayTagHelper()
{
    /*  Damage type resistances map */
    
    ResistanceTagMap.Add(AuraGameplayTags::Damage_Physical, AuraGameplayTags::Attributes_Resistance_Physical);
    ResistanceTagMap.Add(AuraGameplayTags::Damage_Fire, AuraGameplayTags::Attributes_Resistance_Fire);
    ResistanceTagMap.Add(AuraGameplayTags::Damage_Lightning, AuraGameplayTags::Attributes_Resistance_Lightning);
    ResistanceTagMap.Add(AuraGameplayTags::Damage_Arcane, AuraGameplayTags::Attributes_Resistance_Arcane);
}
