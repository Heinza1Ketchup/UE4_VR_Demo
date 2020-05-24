// Fill out your copyright notice in the Description page of Project Settings.


#include "Misc/PickUp.h"
#include "MotionControllerComponent.h"
#include "Components/BoxComponent.h"

// Sets default values
APickUp::APickUp()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	Base = CreateDefaultSubobject<UBoxComponent>(TEXT("Base"));
	Base->SetSimulatePhysics(true);
	//Base->SetBoxExtent(FVector(1, 1, 1));
	Base->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	Base->SetupAttachment(Root);

	InteractiveCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("Interactive Collider"));
	InteractiveCollider->SetupAttachment(Base);

	PickupSnapLocator = CreateDefaultSubobject<USceneComponent>(TEXT("PickupSnapLocator"));
	PickupSnapLocator->SetupAttachment(Base);

	bUsing = false;
	bEnablePickup = true;

	bUsePickupLocator = false;

	bSnapToInitialTransform = false;
	WaitTimeToSnapToInitial = 5.0f;
	CurrentTimeToSnapToInitial = 0.0f;
	bWaitToSnapToInitial = false;

}

// Called when the game starts or when spawned
void APickUp::BeginPlay()
{
	Super::BeginPlay();
	
	bSimulatePhysics = Base->IsSimulatingPhysics();

	InitialBaseTransform = Base->GetComponentTransform();
}

// Called every frame
void APickUp::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bWaitToSnapToInitial)
	{
		CurrentTimeToSnapToInitial += DeltaTime;

		if (CurrentTimeToSnapToInitial >= WaitTimeToSnapToInitial)
		{
			CurrentTimeToSnapToInitial = 0.0f;

			bWaitToSnapToInitial = false;

			Base->SetWorldTransform(InitialBaseTransform);

			bEnablePickup = true;
		}
	}
}

void APickUp::Pickup_Implementation(UMotionControllerComponent* MotionController)
{
	if (!bUsing)
	{
		bUsing = true;

		CurrentMotionController = MotionController;

		Base->SetSimulatePhysics(false);

		// Use the PickupLocator to set the location before attach
		if (bUsePickupLocator)
		{
			FVector SnapLocation = PickupSnapLocator->GetComponentTransform().GetLocation() - Base->GetComponentTransform().GetLocation();
			float SnapDistance = SnapLocation.Size();

			FVector NewLocation = (MotionController->GetForwardVector() * SnapDistance) + MotionController->GetComponentTransform().GetLocation();

			Base->SetWorldLocation(NewLocation);
		}

		Base->AttachToComponent(MotionController, FAttachmentTransformRules::KeepWorldTransform, NAME_None);
	}
}

void APickUp::Drop_Implementation(UMotionControllerComponent* MotionController)
{

	if (!bEnablePickup) return;

	if ((MotionController == nullptr) || (CurrentMotionController == MotionController))
	{
		bUsing = false;

		Base->SetSimulatePhysics(bSimulatePhysics);

		CurrentMotionController = nullptr;

		Base->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);


		// Check if we should snap the pickup to its initial transform after drop
		if (bSnapToInitialTransform)
		{
			bWaitToSnapToInitial = true;
			CurrentTimeToSnapToInitial = 0.0f;

			bEnablePickup = false;
		}
	}

}