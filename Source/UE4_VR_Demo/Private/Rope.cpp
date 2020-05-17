// Fill out your copyright notice in the Description page of Project Settings.


#include "Rope.h"
#include "CableComponent.h"
#include "Components/BoxComponent.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "MotionControllerComponent.h"

ARope::ARope()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CableComponent = CreateDefaultSubobject<UCableComponent>(TEXT("CableComponent"));
	CableComponent->SetupAttachment(Root);

	ConstraintComponent = CreateDefaultSubobject<UPhysicsConstraintComponent>(TEXT("ConstraintComponent"));
	ConstraintComponent->SetupAttachment(Root);

}

void ARope::BeginPlay()
{
	Super::BeginPlay();
	
	InitialCableLength = GetCableLength();
}

void ARope::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(!bUsing) { return; }

	float CurrentCableLength = GetCableLength() - InitialCableLength;

	if (!bIsPulling) {
		if (CurrentCableLength >= MinPullLength) {
			bIsPulling = true;

			OnPull();

			Drop(CurrentMotionController);
		}
	}
}

float ARope::GetCableLength()
{
	float length = FVector::Dist(ConstraintComponent->GetComponentLocation(), Base->GetComponentLocation());

	return length;
}

void ARope::Pickup_Implementation(UMotionControllerComponent *MotionController)
{
	Super::Pickup_Implementation(MotionController);

	bIsPulling = false;
}
void ARope::Drop_Implementation(UMotionControllerComponent *MotionController)
{
	Super::Drop_Implementation(MotionController);

	bIsPulling = false;
}