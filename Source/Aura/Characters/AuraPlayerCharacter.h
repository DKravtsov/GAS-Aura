// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "Characters/AuraCharacterBase.h"
#include "Player/PlayerInterface.h"
#include "AuraPlayerCharacter.generated.h"

/**
 *
 */
UCLASS()
class AURA_API AAuraPlayerCharacter : public AAuraCharacterBase, public IPlayerInterface
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<class USpringArmComponent> CameraBoom;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<class UCameraComponent> CameraComponent;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effects", meta = (AllowPrivateAccess = "true"))
    TSubclassOf<UGameplayEffect> LevelUpEffect;

    UPROPERTY(EditDefaultsOnly, Category = "Effects")
    TSubclassOf<UGameplayEffect> PrimaryAttributes_SetByCaller;

private:
    uint8 bLoadingFromDisk:1 = false;
    
public:

    AAuraPlayerCharacter();

    USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
    UCameraComponent* GetCameraComponent() const { return CameraComponent; }

    //~ Begin APawn Interface
    virtual void PossessedBy(AController* NewController) override;
    virtual void OnRep_PlayerState() override;
    //~End APawn Interface

    //~ Begin of ICombatInterface interface
    virtual int32 BP_GetCharacterLevel_Implementation() const override;
    virtual EAuraCharacterClass GetCharacterClass_Implementation() const override;
    //~ End of ICombatInterface interface

    //~ Begin of IPlayerInterface interface
    virtual void AddXP_Implementation(int32 Amount) override;
    virtual void NotifyLevelUp_Implementation() override;
    virtual void ShowMagicCircle_Implementation(UMaterialInterface* DecalMaterial = nullptr) override;
    virtual void HideMagicCircle_Implementation() override;
    virtual void SaveProgress_Implementation(FName CheckpointTag) override;
    //~ End of IPlayerInterface

protected:

    void LoadProgress();

    virtual void InitializeDefaultPrimaryAttributes() override;
    
private:
    void InitAbilitySystemComponent();
    void InitOverlay();

    mutable int32 CachedPlayerLevel = 1;
};
