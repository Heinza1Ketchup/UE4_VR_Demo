// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PickUp.h"
#include "Pistol.generated.h"

/**
 * 
 */
UCLASS()
class UE4_VR_DEMO_API APistol : public APickUp
{
	GENERATED_BODY()
	

public:
	APistol();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* MyMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneComponent* FireStartingPoint;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
