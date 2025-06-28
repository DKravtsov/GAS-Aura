// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InventoryPlayerController.generated.h"

/**
 * 
 */
UCLASS(MinimalAPI)
class AInventoryPlayerController : public APlayerController
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory", meta=(AllowPrivateAccess=true))
	TObjectPtr<class UInventoryPlayerControllerComponent> InventoryComp;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TObjectPtr<class UInputMappingContext> DefaultInputMappingContext;
	

public:

	INVENTORY_API AInventoryPlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	INVENTORY_API virtual void BeginPlay() override;
};
