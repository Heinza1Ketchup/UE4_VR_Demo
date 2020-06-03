// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ObjectInteractable.generated.h"

class UCameraComponent;
class ACharacter;
class USphereComponent;
class USoundCue;

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* MyMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneComponent* SceneComp;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	USoundCue* SwordOnSoundCue;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	USoundCue* SwordOffSoundCue;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void PickUp();

	UFUNCTION()
	void Drop();

	UFUNCTION()
	void Interact();

	bool bHolding;
	bool bGravity;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bSwordisOn;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bHoldingSword;

	FRotator ControlRotation;
	ACharacter* MyCharacter;
	UCameraComponent* PlayerCamera;
	FVector ForwardVector;
};
