// Fill out your copyright notice in the Description page of Project Settings.

#include "VRCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "NavigationSystem.h"
/* VR Includes */
#include "HeadMountedDisplay.h"
#include "MotionControllerComponent.h"

// Sets default values
AVRCharacter::AVRCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	VROriginComp = CreateDefaultSubobject<USceneComponent>(TEXT("VROriginComp"));
	VROriginComp->SetupAttachment(GetRootComponent());

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	Camera->SetupAttachment(VROriginComp);
	
	DestinationMarker = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DestinationMarker"));
	DestinationMarker->SetupAttachment(GetRootComponent());



	SprintSpeedMultiplier = 1.55f;
}

// Called when the game starts or when spawned
void AVRCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	//teleportation ring toggle
	DestinationMarker->SetVisibility(false);
	TeleportHeld = false;
}

// Called to bind functionality to input
void AVRCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AVRCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AVRCharacter::MoveRight);
	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AVRCharacter::Sprint);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AVRCharacter::StopSprinting);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AVRCharacter::Jump);
	PlayerInputComponent->BindAction("Teleport", IE_Pressed, this, &AVRCharacter::BeginTeleport);
	PlayerInputComponent->BindAction("Teleport", IE_Released, this, &AVRCharacter::EndTeleport);
}

void AVRCharacter::MoveForward(float Value)
{
	//AddActorWorldOffset(Camera->GetForwardVector() * Value);
	AddMovementInput(Camera->GetForwardVector() * Value);
}
void AVRCharacter::MoveRight(float Value)
{
	//AddActorWorldOffset(Camera->GetRightVector() * Value);
	AddMovementInput(Camera->GetRightVector() * Value);
}
void AVRCharacter::Sprint()
{
	GetCharacterMovement()->MaxWalkSpeed *= SprintSpeedMultiplier;
}
void AVRCharacter::StopSprinting()
{
	GetCharacterMovement()->MaxWalkSpeed /= SprintSpeedMultiplier;
}

// Called every frame
void AVRCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//
	
	FVector NewCameraOffset = Camera->GetComponentLocation() - GetActorLocation();
	NewCameraOffset.Z = 0;
	AddActorWorldOffset(NewCameraOffset);


	VROriginComp->AddWorldOffset(-(NewCameraOffset));
	
	//
	if (TeleportHeld) {
		UpdateDestinationMarker();
	}
	
}
bool AVRCharacter::FindTeleportDestination(FVector &OutLocation)
{
	FVector Start = Camera->GetComponentLocation();
	FVector End = Start + Camera->GetForwardVector() * MaxTeleportDistance;
	FHitResult HitResult;

	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility);
	if(!bHit) { return false; }

	FNavLocation NavLocation;
	//GetWorld()->GetNavigationSystem()->ProjectPointToNavigation(HitResult.Location, NavLocation, TeleportProjectExtent);
	bool bOnNavMesh = UNavigationSystemV1::GetCurrent(GetWorld())->ProjectPointToNavigation(HitResult.Location, NavLocation, TeleportProjectExtent);
	if(!bOnNavMesh){ return false; }

	OutLocation = NavLocation.Location;
	return bOnNavMesh && bHit;
}

void AVRCharacter::UpdateDestinationMarker()
{
	FVector Location;
	//
	bool bHasDestination = FindTeleportDestination(Location);
	
	if (bHasDestination)
	{
		DestinationMarker->SetVisibility(true);
		DestinationMarker->SetWorldLocation(Location);
	}
	else {
		DestinationMarker->SetVisibility(false);
	}
}
void AVRCharacter::BeginTeleport()
{
	TeleportHeld = true;
}
void AVRCharacter::EndTeleport()
{
	TeleportHeld = false;
	DestinationMarker->SetVisibility(false);

	//start fade
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC != nullptr) {
		PC->PlayerCameraManager->StartCameraFade(0, 1, TeleportFadeTime, FLinearColor::Black);
	}
	//call settimer() to finish teleport
	FTimerHandle Handle;
	GetWorldTimerManager().SetTimer(Handle, this, &AVRCharacter::FinishTeleport, TeleportFadeTime);
	
}
void AVRCharacter::FinishTeleport()
{
	SetActorLocation(DestinationMarker->GetComponentLocation() + GetCapsuleComponent()->GetScaledCapsuleHalfHeight());

	//start fade
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC != nullptr) {
		PC->PlayerCameraManager->StartCameraFade(1, 0, TeleportFadeTime, FLinearColor::White);
	}
}