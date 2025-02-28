// Copyright 4sandwiches

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MagicCircle.generated.h"

UCLASS()
class AURA_API AMagicCircle : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = MagicCircle, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UDecalComponent> MagicCircleDecal;
	
public:	
	AMagicCircle();

	void SetDecalMaterial(UMaterialInterface* DecalMaterial);
	
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

};
