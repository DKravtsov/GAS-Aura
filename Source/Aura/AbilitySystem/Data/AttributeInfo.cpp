// Copyright 4sandwiches


#include "AbilitySystem/Data/AttributeInfo.h"

FAuraAttributeInfo UAttributeInfo::FindAttributeInfoByTag(const FGameplayTag& AttributeTag, bool bLogNotFound) const
{
    if (const FAuraAttributeInfo* Info = AttributeInfo.FindByKey(AttributeTag))
    {
        return *Info;
    }

    if (bLogNotFound)
    {
        UE_LOG(LogTemp, Error, TEXT("Cannot find info for the AttributeTag [%s] on the AttributeInfo [%s]"),
            *AttributeTag.ToString(), *GetNameSafe(this));
    }

    return FAuraAttributeInfo();
}
