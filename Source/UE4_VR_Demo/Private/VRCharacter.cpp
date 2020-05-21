// Fill out your copyright notice in the Description page of Project Settings.

#include "VRCharacter.h"
#include "HandController.h"
#include "UE4_VR_Demo.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
//#include "Components/SphereComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "NavigationSystem.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "DrawDebugHelpers.h"
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

	UGameplayStatics::SpawnSoundAttached(FireSoundCue, RootComponent);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	Camera->SetupAttachment(VROriginComp);

	/*LeftController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("LeftController"));
	LeftController->SetupAttachment(VROriginComp);
	LeftController->SetTrackingSource(EControllerHand::Left);
	RightController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("RightController"));
	RightController->SetupAttachment(VROriginComp);
	RightController->SetTrackingSource(EControllerHand::Right);
	*/
	//TeleportPath = CreateDefaultSubobject<USplineComponent>(TEXT("TeleportPath"));
	//TeleportPath->SetupAttachment(VROriginComp);

	//
	SprintSpeedMultiplier = 1.55f;

}

// Called when the game starts or when spawned
void AVRCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	
	RightController = GetWorld()->SpawnActor<AHandController>(HandControllerClass);
	if (RightController != nullptr) {
		RightController->AttachToComponent(VROriginComp, FAttachmentTransformRules::KeepRelativeTransform);
		RightController->SetOwner(this);
		RightController->SetHand(EControllerHand::Right);
	}
	LeftController = GetWorld()->SpawnActor<AHandController>(HandControllerClass);
	if (LeftController != nullptr) {
		LeftController->AttachToComponent(VROriginComp, FAttachmentTransformRules::KeepRelativeTransform);
		LeftController->SetOwner(this);
		LeftController->SetHand(EControllerHand::Left);
		LeftController->SetActorRelativeScale3D(FVector(1, -1, 1));
	}
	
	LeftController->PairController(RightController);

	//Teleportation ring toggle
	TeleportHeld = false;

	//DestinationMarker->SetVisibility(false);
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
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AVRCharacter::Fire);
	PlayerInputComponent->BindAction("GrabLeft", IE_Pressed, this, &AVRCharacter::GripLeft);
	PlayerInputComponent->BindAction("GrabLeft", IE_Released, this, &AVRCharacter::ReleaseLeft);
	PlayerInputComponent->BindAction("GrabRight", IE_Pressed, this, &AVRCharacter::GripRight);
	PlayerInputComponent->BindAction("GrabRight", IE_Released, this, &AVRCharacter::ReleaseRight);
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
	else {
		DestinationMarker->SetVisibility(false);

		//TArray<FVector> EmptyPath;
		//DrawTeleportPath(EmptyPath);
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
		
		//UpdateSplineTeleport(Path);
		//DrawTeleportPath(Path);
	}
	/*else {
		DestinationMarker->SetVisibility(false);

		TArray<FVector> EmptyPath;
		DrawTeleportPath(EmptyPath);
	}*/
}
bool AVRCharacter::FindTeleportDestination(TArray<FVector> &OutPath, FVector &OutLocation)
{

	FVector Start = RightController->GetActorLocation();
	FVector Look = RightController->GetActorForwardVector();
	//FVector Start = RightController->GetComponentLocation();
	//FVector Look = RightController->GetForwardVector();
	
	//Look = Look.RotateAngleAxis(30, RightController->GetRightVector());
	//FVector End = Start + Look * MaxTeleportDistance;

	FPredictProjectilePathParams Params(TeleportProjectileRadius, Start, Look * TeleportProjectileSpeed, TeleportSimulationTime, ECollisionChannel::ECC_Visibility, this);
	//Params.DrawDebugType = EDrawDebugTrace::ForOneFrame;
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
void AVRCharacter::DrawTeleportPath(const TArray<FVector> &Path)
{
	UpdateSplineTeleport(Path);

	for (USplineMeshComponent* SplineMesh : TeleportPathMeshPool) {
		SplineMesh->SetVisibility(false);
	}

	int32 SegmentNum = Path.Num() - 1;
	for (int32 i = 0; i < SegmentNum; ++i) {
		if (TeleportPathMeshPool.Num() <= i) {
			USplineMeshComponent* SplineMesh = NewObject<USplineMeshComponent>(this);
			SplineMesh->SetMobility(EComponentMobility::Movable);
			SplineMesh->AttachToComponent(TeleportPath, FAttachmentTransformRules::KeepRelativeTransform);
			SplineMesh->SetStaticMesh(TeleportArchMesh);
			SplineMesh->SetMaterial(0, TeleportArchMat);
			SplineMesh->RegisterComponent();
			TeleportPathMeshPool.Add(SplineMesh);
		}
		USplineMeshComponent* SplineMesh = TeleportPathMeshPool[i];
		SplineMesh->SetVisibility(true);

		FVector StartPos, StartTangent, EndPos, EndTangent;
		TeleportPath->GetLocalLocationAndTangentAtSplinePoint(i, StartPos, StartTangent);
		TeleportPath->GetLocalLocationAndTangentAtSplinePoint(i + 1, EndPos, EndTangent);
		SplineMesh->SetStartAndEnd(StartPos, StartTangent, EndPos, EndTangent, true);
	}
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
	FVector Destination = DestinationMarker->GetComponentLocation();
	Destination += GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * GetActorUpVector();
	SetActorLocation(Destination);

	//start fade
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC != nullptr) {
		PC->PlayerCameraManager->StartCameraFade(1, 0, TeleportFadeTime, FLinearColor::White);
	}
}
//
void AVRCharacter::Fire()
{
/*
	FHitResult HitResult;
	FVector Start = RightController->GetComponentLocation();
	FVector Look = RightController->GetForwardVector();
	FVector End = Start + Look * BulletRange;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	QueryParams.bTraceComplex = true;
	QueryParams.bReturnPhysicalMaterial = true;

	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, COLLISION_WEAPON, QueryParams);

	if (bHit) {
		AActor* Hit = HitResult.GetActor();
		EPhysicalSurface SufraceType = UPhysicalMaterial::DetermineSurfaceType(HitResult.PhysMaterial.Get());


		DrawDebugLine(GetWorld(), Start, End, FColor::White, false, 1.0f, 0, 1.0f);

		if (Hit->IsRootComponentMovable()) {
					
		}
		UGameplayStatics::PlaySoundAtLocation(this, FireSoundCue, GetActorLocation());
	}
	*/
}
