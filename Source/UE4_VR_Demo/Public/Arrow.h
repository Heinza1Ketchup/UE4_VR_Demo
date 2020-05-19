// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PickUp.h"
#include "Arrow.generated.h"

class USceneComponent;

/**
 * 
 */
UCLASS()
class UE4_VR_DEMO_API AArrow : public APickUp
{
	GENERATED_BODY()
	

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Arrow")
	class UStaticMeshComponent* Mesh;

public:

	AArrow();


	void AttachToObject(USceneComponent* Object, FVector OffsetPosition);

	void UpdateXLocation(float Xlocation);

	//Shoot arrow
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Arrow")
	void OnShoot(float Speed);

	void OnShoot_Implementation(float Force);

protected: 
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Arrow");
	float DelayToDestroy = 3.0f;

	FTimerHandle DestroyArrowTimerHandle;

	UFUNCTION()
	void OnDestroyedArrow();

	float ForceArrow;
};
