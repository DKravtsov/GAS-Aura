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

    UE_DEFINE_GAMEPLAY_TAG_COMMENT(Effects_HitReact, "Effects.HitReact", "");

    UE_DEFINE_GAMEPLAY_TAG_COMMENT(Damage, "Damage", "Parent tag for all damage types");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(Damage_Physical, "Damage.Physical", "Physical damage type");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(Damage_Fire, "Damage.Fire", "Fire damage type");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(Damage_Lightning, "Damage.Lightning", "Lightning damage type");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(Damage_Arcane, "Damage.Arcane", "Arcane damage type");

    UE_DEFINE_GAMEPLAY_TAG_COMMENT(Abilities, "Abilities", "Parent tag for all gameplay abilities");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(Abilities_Attack, "Abilities.Attack", "Parent tag for all attacking abilities");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(Abilities_Attack_Melee, "Abilities.Attack.Melee", "Melee attack");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(Abilities_Attack_Ranged, "Abilities.Attack.Ranged", "Ranged attack");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(Abilities_Attack_Spell, "Abilities.Attack.Spell", "Magic spell cast");

    UE_DEFINE_GAMEPLAY_TAG_COMMENT(Abilities_Summon, "Abilities.Summon", "Ability to summon minions");

    UE_DEFINE_GAMEPLAY_TAG_COMMENT(CombatSocket_Weapon, "CombatSocket.Weapon", "Get socket location from the equipped weapon");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(CombatSocket_LeftHand, "CombatSocket.LeftHand", "get socket location from the character's mesh left hand");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(CombatSocket_RightHand, "CombatSocket.RightHand", "get socket location from the character's mesh right hand");

    UE_DEFINE_GAMEPLAY_TAG_COMMENT(Cooldown, "Cooldown", "Parent tag for all cooldown tags");
    
}

FGameplayTagContainer FGameplayTagHelper::RequestAllDamageTypeGameplayTags()
{
    return UGameplayTagsManager::Get().RequestGameplayTagChildren(AuraGameplayTags::Damage);
}

FGameplayTagContainer FGameplayTagHelper::RequestAllResistanceGameplayTags()
{
    return UGameplayTagsManager::Get().RequestGameplayTagChildren(AuraGameplayTags::Attributes_Resistance);
}

FGameplayTag FGameplayTagHelper::GetResistanceTagByDamageType(FGameplayTag DamageTypeTag)
{
    return Get().ResistanceTagMap.FindChecked(DamageTypeTag);
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
