// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryPlayerControllerComponent.generated.h"

class UInputAction;

UCLASS(MinimalAPI, ClassGroup=(Inventory), meta=(BlueprintSpawnableComponent))
class UInventoryPlayerControllerComponent : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TObjectPtr<UInputAction> PrimaryInteractionAction;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TSubclassOf<class UInventoryHUDWidget> HUDWidgetClass;

	UPROPERTY(Transient)
	TObjectPtr<UInventoryHUDWidget> HUDWidget;

	TWeakObjectPtr<AActor> CurrentInteractableActor;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TEnumAsByte<ECollisionChannel> InteractionTraceChannel;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	float InteractionTraceDistance;
	
public:

	INVENTORY_API UInventoryPlayerControllerComponent();
	
	INVENTORY_API virtual void SetupInputComponent(class UEnhancedInputComponent* InputComponent);

	template <class TController>
	TController* GetOwningController() const {return Cast<TController>(GetOwner());}

	UFUNCTION(BlueprintCallable)
	APlayerController* GetOwningPlayerController() const {return Cast<APlayerController>(GetOwner());}

	FORCEINLINE_DEBUGGABLE
	bool IsLocallyControlled() const
	{
		if (const auto PC = GetOwningPlayerController())
		{
			return PC->IsLocalController();
		}
		return false;
	}

	// Performs a single line trace from the center of the current camera, and calls UpdateInteractionTraceResult()
	INVENTORY_API virtual void PerformInteractionTraceFromCamera();
	
	// Performs a single line trace under the mouse cursor, and calls UpdateInteractionTraceResult()
	INVENTORY_API virtual void PerformInteractionTraceUnderMouse();

	// Update interactable actor. Will be called from PerformInteractionTraceXXX() methods or can be called directly instead if applicable
	INVENTORY_API virtual void UpdateInteractionTraceResult(AActor* InteractableActor);

protected:

	INVENTORY_API virtual void BeginPlay() override;

private:
	void PrimaryInteract();
	void CreateHUDWidget();
};
