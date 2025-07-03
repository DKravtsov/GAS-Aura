// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryPlayerControllerComponent.generated.h"

class UInputAction;

/*
 * A player controller component responsible for basic input and tracking of what can be picked up.
 * Also, can open / close Inventory menu.
 * Setup: Component must be added to PlayerController, all needed properties should be defined.
 * Besides that, the owning player controller should call one of
 * PerformInteractionTraceFromCamera() or PerformInteractionTraceUnderMouse() (depending on the game type),
 * or directly call UpdateInteractionTraceResult() if it already has its own detection.
 */
UCLASS(MinimalAPI, ClassGroup=(Inventory), meta=(BlueprintSpawnableComponent), Within=PlayerController)
class UInventoryPlayerControllerComponent : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TObjectPtr<UInputAction> PrimaryInteractionAction;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TObjectPtr<UInputAction> ToggleInventoryAction;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TSubclassOf<class UInventoryHUDWidget> HUDWidgetClass;

	UPROPERTY(Transient)
	TObjectPtr<UInventoryHUDWidget> HUDWidget;

	TWeakObjectPtr<AActor> CurrentInteractableActor;
	TWeakObjectPtr<class UInventoryComponent> InventoryComponent;

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

	UInventoryComponent* GetInventoryComponent() const {return InventoryComponent.Get();}

protected:

	INVENTORY_API virtual void BeginPlay() override;

private:
	void PrimaryInteract();
	void CreateHUDWidget();
	void ToggleInventory();

	UFUNCTION()
	void OnInventoryMenuOpened();

	UFUNCTION()
	void OnInventoryMenuClosed();
};
