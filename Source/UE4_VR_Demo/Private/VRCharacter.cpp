// Fill out your copyright notice in the Description page of Project Settings.

#include "VRCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SplineComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "NavigationSystem.h"
#include "Kismet/GameplayStatics.h"
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

	DestinationMarker = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DestinationMarker"));
	DestinationMarker->SetupAttachment(GetRootComponent());

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	Camera->SetupAttachment(VROriginComp);
	//
	LeftController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("LeftController"));
	LeftController->SetupAttachment(VROriginComp);
	LeftController->SetTrackingSource(EControllerHand::Left);
	//LeftController->bDisplayDeviceModel = true;
	RightController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("RightController"));
	RightController->SetupAttachment(VROriginComp);
	RightController->SetTrackingSource(EControllerHand::Right);
	//RightController->bDisplayDeviceModel = true;


	TeleportPath = CreateDefaultSubobject<USplineComponent>(TEXT("Destination"));
	TeleportPath->SetupAttachment(RightController);


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

//Teleportation destination 
void AVRCharacter::UpdateDestinationMarker()
{
	TArray<FVector> Path;
	FVector Location;
	//
	bool bHasDestination = FindTeleportDestination(Path, Location);

	if (bHasDestination)
	{
		DestinationMarker->SetVisibility(true);
		DestinationMarker->SetWorldLocation(Location);
		UpdateSplineTeleport(Path);
	}
	else {
		DestinationMarker->SetVisibility(false);
	}
}
bool AVRCharacter::FindTeleportDestination(TArray<FVector> &OutPath, FVector &OutLocation)
{
	FVector Start = RightController->GetComponentLocation();
	FVector Look = RightController->GetForwardVector();
	//FVector End = Start + Look * MaxTeleportDistance;

	FPredictProjectilePathParams Params(TeleportProjectileRadius, Start, Look * TeleportProjectileSpeed, TeleportSimulationTime, ECollisionChannel::ECC_Visibility, this);
	Params.DrawDebugType = EDrawDebugTrace::ForOneFrame;
	//Params.bTraceComplex = true; //optional for testing

	//FHitResult HitResult;
	//bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility);
	FPredictProjectilePathResult Result;
	bool bHit = UGameplayStatics::PredictProjectilePath(this, Params, Result);

	if (!bHit) { return false; }

	for (FPredictProjectilePathPointData PointData : Result.PathData)
	{
		OutPath.Add(PointData.Location);
	}

	FNavLocation NavLocation;
	//GetWorld()->GetNavigationSystem()->ProjectPointToNavigation(HitResult.Location, NavLocation, TeleportProjectExtent);
	bool bOnNavMesh = UNavigationSystemV1::GetCurrent(GetWorld())->ProjectPointToNavigation(Result.HitResult.Location, NavLocation, TeleportProjectExtent);

	if (!bOnNavMesh) { return false; }

	OutLocation = NavLocation.Location;
	return bOnNavMesh && bHit;
}
void AVRCharacter::UpdateSplineTeleport(const TArray<FVector> &Path)
{
	TeleportPath->ClearSplinePoints(false);
	for (int32 i = 0; i < Path.Num(); i++)
	{
		FVector LocalPosition = TeleportPath->GetComponentTransform().InverseTransformPosition(Path[i]);
		FSplinePoint Point(i, LocalPosition, ESplinePointType::Curve);
		TeleportPath->AddPoint(Point, false);

	}
	TeleportPath->UpdateSpline();
}
//
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
