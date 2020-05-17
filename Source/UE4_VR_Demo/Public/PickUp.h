// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PickupInterface.h"
#include "PickUp.generated.h"


class UMotionControllerComponent;
class USceneComponent;


UCLASS()
class UE4_VR_DEMO_API APickUp : public AActor, public IPickupInterface
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Pickup")
	USceneComponent *Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Pickup")
	class UBoxComponent* Base;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Pickup")
	class UBoxComponent* InteractiveCollider;

public:	
	// Sets default values for this actor's properties
	APickUp();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Pickup Interface")
	void Pickup(UMotionControllerComponent *MotionController);

	virtual void Pickup_Implementation(UMotionControllerComponent *MotionController) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Pickup Interface")
	void Drop(UMotionControllerComponent *MotionController);

	virtual void Drop_Implementation(UMotionControllerComponent *MotionController) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Pickup")
	bool bUsing;

	bool bSimulatePhysics;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Pickup")
	class UMotionControllerComponent *CurrentMotionController;
};
