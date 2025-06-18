// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryPlayerControllerComponent.generated.h"

class UInputAction;

UCLASS(MinimalAPI, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UInventoryPlayerControllerComponent : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TObjectPtr<UInputAction> PrimaryInteractionAction;

public:

	INVENTORY_API UInventoryPlayerControllerComponent();
	
	INVENTORY_API virtual void SetupInputComponent(class UEnhancedInputComponent* InputComponent);

protected:

	INVENTORY_API virtual void BeginPlay() override;

private:
	void PrimaryInteract();
};
