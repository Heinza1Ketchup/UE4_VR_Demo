// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MotionControllerComponent.h"

#include "HandController.generated.h"


class UHapticFeedbackEffect_Base;

UCLASS()
class UE4_VR_DEMO_API AHandController : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHandController();

	void SetHand(EControllerHand Hand) { 
		//MotionController->Hand_DEPRECATED = Hand;
		MotionController->SetTrackingSource(Hand);
	}
	void PairController(AHandController* Controller);

	void Grip();
	void Release();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool ActivateHFB;

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
	bool bCanClimb = false;
	bool bIsClimbing = false;
	FVector ClimbingStartLocation;
	
	AHandController* OtherController;

	//helpers
	bool CanClimb() const;
};
