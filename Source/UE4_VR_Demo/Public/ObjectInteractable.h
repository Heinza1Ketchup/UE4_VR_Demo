// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ObjectInteractable.generated.h"

class UCameraComponent;
class ACharacter;
class USphereComponent;

UCLASS()
class UE4_VR_DEMO_API AObjectInteractable : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AObjectInteractable();

	UPROPERTY(EditAnywhere)
	FString ObjectName;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* MyMesh;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void PickUp();

	UFUNCTION()
	void Drop();

	bool bHolding;
	bool bGravity;


	FRotator ControlRotation;
	ACharacter* MyCharacter;
	UCameraComponent* PlayerCamera;
	FVector ForwardVector;
};
