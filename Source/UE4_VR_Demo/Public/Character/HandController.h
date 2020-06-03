// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MotionControllerComponent.h"
#include "HandController.generated.h"


class UHapticFeedbackEffect_Base;
class UObjectInteractable;
class USphereComponent;

UCLASS()
class UE4_VR_DEMO_API AHandController : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHandController();

	void SetHand(EControllerHand Hand) { 
		MotionController->SetTrackingSource(Hand);
		MotionController->Hand_DEPRECATED = Hand;
	}
	void PairController(AHandController* Controller);

	void Grip();
	void Release();

	void Interact();

	void WeaponFire();
	void WeaponStop();

	void SetStateFalling();
	FTimerHandle TimerHandle_ClimbReleaseTime;

	//inspect
	UPROPERTY(EditAnywhere)
	class USceneComponent* HoldingComponent;

	UPROPERTY(EditAnywhere)
	class AObjectInteractable* CurrentItem;

	UPROPERTY(EditAnywhere)
	class AObjectInteractable* ForceItem;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class USphereComponent* ForceSphere;

	UPROPERTY(EditAnywhere)
	class AWeapon* CurrentWeapon;

	bool bHoldingItem;
	bool bInspecting;

	FComponentQueryParams DefaultQueryParams;
	FCollisionResponseParams DefaultReponseParams;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool ActivateHFB;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bObjectInHand = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bWeaponinHand = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bSwordInHand = false;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	//default subobject
	UPROPERTY(VisibleAnywhere)
	UMotionControllerComponent* MotionController;

	//parameters
	UPROPERTY(EditDefaultsOnly)
	UHapticFeedbackEffect_Base * HapticEffect;

	

	//calbacks
	UFUNCTION()
	void ActorBeginOverlap(AActor* OverlappedActor, AActor* OtherActor);
	UFUNCTION()
	void ActorEndOverlap(AActor* OverlappedActor, AActor* OtherActor);

	//state
	bool bCanReload = false;
	bool bCanPickup = false;
	bool bCanFly = false;
	bool bIsFlying = false;
	bool bCanClimb = false;
	bool bIsClimbing = false;
	FVector ClimbingStartLocation;
	
	AHandController* OtherController;

	//helpers
	bool CanClimb() const;
	bool CanFly() const;
	bool CanPickup();
	bool CanReload();

	float BulletRange = 9999.f;
};
 