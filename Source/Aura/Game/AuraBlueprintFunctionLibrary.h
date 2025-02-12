// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AuraBlueprintFunctionLibrary.generated.h"

/**
 *
 */
UCLASS()
class AURA_API UAuraBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:

    UFUNCTION(BlueprintPure, Category = "AuraAbilitySystem|WidgetController", BlueprintCosmetic, meta = (WorldContext = "WorldContextObject"))
    static class UOverlayWidgetController* GetOverlayWidgetController(const UObject* WorldContextObject);

    UFUNCTION(BlueprintPure, Category = "AuraAbilitySystem|WidgetController", BlueprintCosmetic, meta = (WorldContext = "WorldContextObject"))
    static class UAttributeMenuWidgetController* GetAttributeMenuWidgetController(const UObject* WorldContextObject);

    UFUNCTION(BlueprintPure, Category = "AuraAbilitySystem|CharacterClassDefaults", meta = (WorldContext = "WorldContextObject"))
    static class UCharacterClassInfo* GetCharacterClassInfo(const UObject* WorldContextObject);



    UFUNCTION(BlueprintPure, Category = "AuraAbilitySystem|CharacterInfo")
    static int32 GetCharacterLevel(const AActor* Actor);
};
