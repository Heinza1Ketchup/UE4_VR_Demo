// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/HandController.h"
#include "MotionControllerComponent.h"
#include "GameFrameWork/Pawn.h"
#include "GameFrameWork/PlayerController.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/BoxComponent.h"
#include "GameActors/ObjectInteractable.h"
#include "GameActors/Weapon.h"


// Sets default values
AHandController::AHandController()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("MotionController"));
	SetRootComponent(MotionController);

	CurrentItem = nullptr;
	CurrentWeapon = nullptr;
	
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
	bWeaponinHand = false;
	bCanReload = false;
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

void AHandController::PairController(AHandController* Controller)
{
	OtherController = Controller;
	OtherController->OtherController = this;
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

	bool bNewCanReload = CanReload();
	bCanReload = bNewCanReload;
}

void AHandController::ActorEndOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	bCanClimb = CanClimb();
	bCanFly  = CanFly();
	bCanPickup = CanPickup();
	bCanReload = CanReload();

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
		}
		else if (OverlappingActor->ActorHasTag(TEXT("Katana"))) {
			CurrentItem = Cast<AObjectInteractable>(OverlappingActor);
			return true;
		}
		else if (OverlappingActor->ActorHasTag(TEXT("Weapon"))) {
			CurrentWeapon = Cast<AWeapon>(OverlappingActor);
			return true;
		}
		else{ 
			CurrentItem = nullptr; 
		}
	}

	return false;
}
bool AHandController::CanReload()
{
	TArray<AActor*> OverlappingActors;
	GetOverlappingActors(OverlappingActors);

	for (AActor* OverlappingActor : OverlappingActors)
	{
		if (OverlappingActor->ActorHasTag(TEXT("Weapon")) && OtherController->bWeaponinHand && !bWeaponinHand) {
			return true;
		}
	}

	return false;
}

void AHandController::Interact()
{

}

void AHandController::Grip()
{
	//
	if (bCanPickup) {
		TArray<AActor*> OverlappingActors;
		GetOverlappingActors(OverlappingActors);

		for (AActor* OverlappingActor : OverlappingActors)
		{
			if (OverlappingActor->ActorHasTag(TEXT("Pickupable")) && !bObjectInHand && !bWeaponinHand) {
				//OverlappingActor->AttachToComponent(MotionController, FAttachmentTransformRules::KeepWorldTransform, NAME_None);
				CurrentItem->PickUp();
				CurrentItem->AttachToComponent(MotionController, FAttachmentTransformRules::KeepWorldTransform, NAME_None);
				bObjectInHand = true;
			}
			if (OverlappingActor->ActorHasTag(TEXT("Katana")) && !bObjectInHand && !bWeaponinHand) {
				CurrentItem->PickUp();
				FAttachmentTransformRules FattachRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, false);
				CurrentItem->AttachToComponent(MotionController, FattachRules, NAME_None);
				FHitResult Fhit;
				CurrentItem->AddActorLocalRotation(FRotator(215.f, 180.f, 0.f), false);
				if (MotionController->Hand_DEPRECATED == EControllerHand::Right) {
					CurrentItem->K2_AddActorLocalOffset(FVector(0.f, 6.f, 0.f), false, Fhit, false);
				}
				else if (MotionController->Hand_DEPRECATED == EControllerHand::Left)
				{
					CurrentItem->K2_AddActorLocalOffset(FVector(0.f, -6.f, 0.f), false, Fhit, false);
				}
				
				bObjectInHand = true;
			}
			if (OverlappingActor->ActorHasTag(TEXT("Weapon")) && !bObjectInHand && !bWeaponinHand) {
				CurrentWeapon->PickUp();
				FAttachmentTransformRules FattachRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, false);
				CurrentWeapon->AttachToComponent(MotionController, FattachRules, NAME_None);
				FHitResult fHit;
				CurrentWeapon->AddActorLocalRotation(FRotator(-12.f, 0.f, 0.f), false);
				FHitResult Fhit;

				bWeaponinHand = true;
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

		GetWorldTimerManager().SetTimer(TimerHandle_ClimbReleaseTime, this, &AHandController::SetStateFalling, 0.1, false);
	}

	if (bObjectInHand) {
		CurrentItem->Drop();
		CurrentItem->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		bObjectInHand = false;
	}
	if(bWeaponinHand){
		CurrentWeapon->StopFire();
		CurrentWeapon->Drop();
		CurrentWeapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		bWeaponinHand = false;	
	}
}

void AHandController::SetStateFalling()
{
	ACharacter* Character = Cast<ACharacter>(GetAttachParentActor());
	if (Character != nullptr) {
		Character->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Falling);
	}
}

void AHandController::WeaponFire()
{
	if (bCanReload) {
		CurrentWeapon->ReloadWeapon();
		return;
	}
	if(!bWeaponinHand){	return;	}

	CurrentWeapon->StartFire();
}

void AHandController::WeaponStop()
{
	if (!bWeaponinHand) {
		return; 
	}

	CurrentWeapon->StopFire();
}