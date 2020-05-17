// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PickUp.h"
#include "Rope.generated.h"


class UPhysicsConstraintComponent;
class UCableComponent;
class UMotionControllerComponent;
/**
 * 
 */
UCLASS()
class UE4_VR_DEMO_API ARope : public APickUp
{
	GENERATED_BODY()
	
public:

	ARope();

	virtual void Tick(float DeltaTime) override;
protected:
	virtual void BeginPlay() override;

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Cable")
	UPhysicsConstraintComponent* ConstraintComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Cable")
	UCableComponent* CableComponent;

protected:
	UPROPERTY(EditDefaultsOnly, Category="Cable")
	float MinPullLength = 25.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Cable")
	bool bIsPulling;

	UFUNCTION(BlueprintCallable, Category="Cable")
	float GetCableLength();

	UFUNCTION(BlueprintImplementableEvent, Category="Cable")
	void OnPull();

	float InitialCableLength;

public:
	virtual void Drop_Implementation(UMotionControllerComponent *MotionController) override;

	virtual void Pickup_Implementation(UMotionControllerComponent *MotionController) override;
};
