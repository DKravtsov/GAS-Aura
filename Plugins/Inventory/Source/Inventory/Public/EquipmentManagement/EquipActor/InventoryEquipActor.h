// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "InventoryEquipActor.generated.h"

UCLASS(MinimalAPI, Blueprintable, Abstract, NotPlaceable)
class AInventoryEquipActor : public AActor
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="Inventory", meta=(Categories="GameItems.Equipment"))
	FGameplayTag EquipmentType;

public:

	AInventoryEquipActor();

	const FGameplayTag& GetEquipmentType() const {return EquipmentType;}
	void SetEquipmentType(const FGameplayTag& NewEquipmentType) {EquipmentType = NewEquipmentType;}

protected:

	virtual void BeginPlay() override;
	
};
