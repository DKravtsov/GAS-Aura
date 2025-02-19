// Copyright 4sandwiches


#include "Characters/CombatInterface.h"

#include "AbilitySystem/Data/CharacterClassInfo.h"

// Add default functionality here for any ICombatInterface functions that are not pure virtual.
EAuraCharacterClass ICombatInterface::GetCharacterClass() const
{
	return EAuraCharacterClass::Undefined;
}
