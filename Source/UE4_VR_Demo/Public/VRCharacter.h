// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "VRCharacter.generated.h"

class UCameraComponent;
class UMotionControllerComponent;
class UStaticMeshComponent;
class UMaterialInterface;
class USoundCue;

UCLASS()
class UE4_VR_DEMO_API AVRCharacter : public ACharacter
{
	GENERATED_BODY()


public:
	// Sets default values for this character's properties
	AVRCharacter();

	//
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Movement: Walking")
	float SprintSpeedMultiplier;

	bool IsSprinting;

	bool TeleportHeld;

protected:
	// Called when the game starts or when spawned	`
	virtual void BeginPlay() override;

	//Player Camera
	UPROPERTY(Visibleanywhere, BlueprintReadOnly, Category = "Components")
	class UCameraComponent* Camera;
	/* Component to specify origin for the HMD */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* VROriginComp;
	/* Component to specify destination marker for Teleportation*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UStaticMeshComponent* DestinationMarker;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TArray<class USplineMeshComponent*> TeleportPathMeshPool;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USceneComponent* ManipulatableR;

	//Player Controllers
	/*UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class AHandController* LeftController;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class AHandController* RightController;	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UMotionControllerComponent* LeftController;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UMotionControllerComponent* RightController;

	//sound
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	USoundCue* FireSoundCue;

	//spline
	UPROPERTY(VisibleAnywhere)
	class USplineComponent* TeleportPath;

	//teleport
	bool FindTeleportDestination(TArray<FVector> &OutPath, FVector &OutLocation);
	void DrawTeleportPath(const TArray<FVector> &Path);
	void UpdateSplineTeleport(const TArray<FVector> &Path);
	void UpdateDestinationMarker();
	void BeginTeleport();
	void EndTeleport();
	void FinishTeleport();
	
	//movement
	void MoveForward(float Value);
	void MoveRight(float Value);

	//sprinting
	void Sprint();
	void StopSprinting();

	//Fire
	void Fire();



public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
			
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	//configuration
	UPROPERTY(EditAnywhere)
	float MaxTeleportDistance = 1000;

	UPROPERTY(EditAnywhere)
	float TeleportProjectileSpeed = 800;

	UPROPERTY(EditAnywhere)
	float TeleportProjectileRadius = 10;

	UPROPERTY(EditAnywhere)
	float TeleportSimulationTime = 2;

	UPROPERTY(EditAnywhere)
	float TeleportFadeTime = 0.25f;

	UPROPERTY(EditAnywhere)
	FVector TeleportProjectExtent = FVector(100.f, 100.f, 100.f);

	UPROPERTY(EditDefaultsOnly)
	class UStaticMesh* TeleportArchMesh;

	UPROPERTY(EditDefaultsOnly)
	class UMaterialInterface* TeleportArchMat;

	UPROPERTY(EditAnywhere)
	float BulletRange = 9999;
	//UPROPERTY(EditDefaultsOnly)
	//TSubclassOf<AHandController> HandControllerClass;
};



