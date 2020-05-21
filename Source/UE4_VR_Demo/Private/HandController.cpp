// Fill out your copyright notice in the Description page of Project Settings.


#include "HandController.h"
#include "MotionControllerComponent.h"
#include "GameFrameWork/Pawn.h"
#include "GameFrameWork/PlayerController.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/BoxComponent.h"
#include "ObjectInteractable.h"

// Sets default values
AHandController::AHandController()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("MotionController"));
	SetRootComponent(MotionController);
	

	HoldingComponent = CreateDefaultSubobject<USceneComponent>(TEXT("HoldingComponent"));
	HoldingComponent->SetupAttachment(MotionController);

	CurrentItem = nullptr;
	bInspecting = false;
}

// Called when the game starts or when spawned
void AHandController::BeginPlay()
{
	Super::BeginPlay();
	
	
	OnActorBeginOverlap.AddDynamic(this, &AHandController::ActorBeginOverlap);
	OnActorEndOverlap.AddDynamic(this, &AHandController::ActorEndOverlap);

	ActivateHFB = false;
	bCanClimb = false;
	bIsClimbing = false;
	bCanFly = false;
	bIsFlying = false;
	bCanPickup = false;
	bObjectInHand = false;
}

// Called every frame
void AHandController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	
	if (bIsClimbing)
	{
		FVector HandControllerDelta = GetActorLocation() - ClimbingStartLocation; 
		GetAttachParentActor()->AddActorWorldOffset(-HandControllerDelta);
	}
}

void AHandController::ActorBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	
	bool bNewCanClimb = CanClimb();
	if (!bCanClimb && bNewCanClimb) {
		APawn* Pawn = Cast<APawn>(GetAttachParentActor());
		if (Pawn != nullptr) {
			APlayerController* Controller = Cast<APlayerController>(Pawn->GetController());
			if (Controller != nullptr)
			{
				
				Controller->PlayHapticEffect(HapticEffect, MotionController->Hand_DEPRECATED);
				ActivateHFB = true;
			}
		}
	}else{ ActivateHFB = false; }

	bCanClimb = bNewCanClimb;

	bool bNewCanFly = CanFly();
	bCanFly = bNewCanFly;

	bool bNewCanPickup = CanPickup();
	bCanPickup = bNewCanPickup;
}
void AHandController::ActorEndOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	bCanClimb = CanClimb();
	bCanFly  = CanFly();
}
bool AHandController::CanClimb() const
{
	TArray<AActor*> OverlappingActors;
	GetOverlappingActors(OverlappingActors);

	for (AActor* OverlappingActor : OverlappingActors)
	{
		if (OverlappingActor->ActorHasTag(TEXT("Climbable"))) {
			return true;
		}
	}

	return false;
}
bool AHandController::CanFly() const
{
	TArray<AActor*> OverlappingActors;
	GetOverlappingActors(OverlappingActors);

	for (AActor* OverlappingActor : OverlappingActors)
	{
		
		if (OverlappingActor->ActorHasTag(TEXT("FlyToggle"))) {
			return true;
		}
	}

	return false;
}
bool AHandController::CanPickup() 
{
	TArray<AActor*> OverlappingActors;
	GetOverlappingActors(OverlappingActors);

	for (AActor* OverlappingActor : OverlappingActors)
	{

		if (OverlappingActor->ActorHasTag(TEXT("Pickupable"))) {
			CurrentItem = Cast<AObjectInteractable>(OverlappingActor);
			return true;
		}else{ CurrentItem = nullptr; }
	}

	return false;
}


void AHandController::Grip()
{
	//
	if (bCanPickup) {
		UE_LOG(LogTemp, Warning, (TEXT("boing")));
		TArray<AActor*> OverlappingActors;
		GetOverlappingActors(OverlappingActors);

		for (AActor* OverlappingActor : OverlappingActors)
		{
			if (OverlappingActor->ActorHasTag(TEXT("Pickupable"))) {
				//OverlappingActor->AttachToComponent(MotionController, FAttachmentTransformRules::KeepWorldTransform, NAME_None);
				CurrentItem->PickUp();
				CurrentItem->AttachToComponent(MotionController, FAttachmentTransformRules::KeepWorldTransform, NAME_None);
				bObjectInHand = true;
			}
		}
	}

	if (bCanFly) {
		ACharacter* Character = Cast<ACharacter>(GetAttachParentActor());
		if (Character != nullptr) {
			if (bIsFlying) {
				Character->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Falling);
				bIsFlying = false;
				OtherController->bIsFlying = false;
			}
			else {
				Character->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
				bIsFlying = true;
				OtherController->bIsFlying = true;
			}

		}
	}

	if (!bIsClimbing) {
		if (!bCanClimb) { return; }
		if (bIsFlying) { return; }
		bIsClimbing = true;
		ClimbingStartLocation = GetActorLocation();

		OtherController->bIsClimbing = false;

		ACharacter* Character = Cast<ACharacter>(GetAttachParentActor());
		if (Character != nullptr) {
			Character->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
		}
	}
}

void AHandController::Release()
{
	if (bIsClimbing) {
		bIsClimbing = false;

		ACharacter* Character = Cast<ACharacter>(GetAttachParentActor());
		if (Character != nullptr) {
			Character->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Falling);
		}
	}

	if (bObjectInHand) {
		TArray<AActor*> OverlappingActors;
		GetOverlappingActors(OverlappingActors);

		for (AActor* OverlappingActor : OverlappingActors)
		{
			if (OverlappingActor->ActorHasTag(TEXT("Pickupable"))) {
				CurrentItem->Drop();
				CurrentItem->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
				bObjectInHand = false;
			}
		}
	}

	
}
void AHandController::PairController(AHandController* Controller)
{
	OtherController = Controller;
	OtherController->OtherController = this;
}