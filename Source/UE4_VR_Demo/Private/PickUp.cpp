// Fill out your copyright notice in the Description page of Project Settings.


#include "PickUp.h"
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

	bUsing = false;

}

// Called when the game starts or when spawned
void APickUp::BeginPlay()
{
	Super::BeginPlay();
	
	bSimulatePhysics = Base->IsSimulatingPhysics();
}

// Called every frame
void APickUp::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APickUp::Pickup_Implementation(UMotionControllerComponent* MotionController)
{
	if (!bUsing) {
		bUsing = true;

		CurrentMotionController = MotionController;

		Base->SetSimulatePhysics(false);
		//Base->AttachToComponent(MotionController, FAttachmentTransformRules::KeepWorldTransform, NAME_None);
		//UE_LOG(LogTemp, Log, (TEXT("Captains log star date")));


	}
}

void APickUp::Drop_Implementation(UMotionControllerComponent* MotionController)
{
	if ((MotionController == nullptr) || (CurrentMotionController == MotionController))
	{
		bUsing = false;

		Base->SetSimulatePhysics(bSimulatePhysics);

		CurrentMotionController = nullptr;
		
		Base->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);

	}
}