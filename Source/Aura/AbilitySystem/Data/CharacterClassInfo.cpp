// Copyright 4sandwiches


#include "AbilitySystem/Data/CharacterClassInfo.h"

FCharacterClassDefaultInfo UCharacterClassInfo::GetClassDefaultInfo(EAuraCharacterClass CharacterClass)
{
    return ClassInfo.FindChecked(CharacterClass);
}
