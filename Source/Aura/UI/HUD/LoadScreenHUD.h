// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "LoadScreenHUD.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API ALoadScreenHUD : public AHUD
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class ULoadScreenWidget> LoadScreenWidgetClass;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<ULoadScreenWidget> LoadScreenWidget;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UMVVMLoadScreen> LoadScreenModelClass;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UMVVMLoadScreen> LoadScreenViewModel;

	
protected:

	virtual void BeginPlay() override;
};
