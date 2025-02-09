// Copyright 4sandwiches


#include "AbilitySystem/Data/AttributeInfo.h"
#include "AttributeSet.h"

FAuraAttributeInfo UAttributeInfo::FindAttributeInfoByTag(const FGameplayTag& AttributeTag, bool bLogNotFound) const
{
    //if (const FAuraAttributeInfo* Info = AttributeInfo.FindByKey(AttributeTag))
    //{
    //    return *Info;
    //}

    for (const auto& [Attribute, Info] : AttributeInfoMap)
    {
        if (Info.AttributeTag.MatchesTagExact(AttributeTag))
        {
            return Info;
        }
    }

    if (bLogNotFound)
    {
        UE_LOG(LogTemp, Error, TEXT("Cannot find info for the AttributeTag [%s] on the AttributeInfo [%s]"),
            *AttributeTag.ToString(), *GetNameSafe(this));
    }

    return FAuraAttributeInfo();
}

FAuraAttributeInfo UAttributeInfo::FindAttributeInfo(const FGameplayAttribute& Attribute, bool bLogNotFound) const
{
    //if (const FGameplayTag* Tag = AttributeMap.Find(Attribute))
    //{
    //    return FindAttributeInfoByTag(*Tag, bLogNotFound);
    //}

    if (const FAuraAttributeInfo* Info = AttributeInfoMap.Find(Attribute))
    {
        return *Info;
    }

    if (bLogNotFound)
    {
        UE_LOG(LogTemp, Error, TEXT("Cannot find info for the Attribute [%s] on the AttributeInfo [%s]"),
            *Attribute.GetName(), *GetNameSafe(this));
    }

    return FAuraAttributeInfo();
}
