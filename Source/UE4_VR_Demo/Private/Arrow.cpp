// Fill out your copyright notice in the Description page of Project Settings.


#include "Arrow.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"


AArrow::AArrow()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ArrowMesh"));
	Mesh->SetupAttachment(Base);

	bEnablePickup = false;
}

void AArrow::AttachToObject(USceneComponent* Object, FVector OffsetPosition)
{
	bEnablePickup = false;
	bSimulatePhysics = false;

	//remove collision from base and mesh
	Base->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Base->SetSimulatePhysics(false);
	Base->SetEnableGravity(false);

	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh->SetSimulatePhysics(false);
	Mesh->SetEnableGravity(false);

	Base->SetRelativeLocation(Object->GetRelativeLocation() + OffsetPosition);
	Base->AttachToComponent(Object, FAttachmentTransformRules::KeepWorldTransform, NAME_None);
}

void AArrow::UpdateXLocation(float Xlocation)
{
	FVector CurrentBaseLocation = Base->GetRelativeLocation();
	CurrentBaseLocation.X = Xlocation;
	Base->SetRelativeLocation(CurrentBaseLocation);
}

void AArrow::OnShoot_Implementation(float Force)
{
	Base->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);

	ForceArrow = Force;

	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Mesh->SetSimulatePhysics(true);
	Mesh->SetEnableGravity(true);

	//apply a force to the mesh, on the forward vector
	Mesh->AddForce(Base->GetForwardVector() * ForceArrow);

	GetWorld()->GetTimerManager().SetTimer(DestroyArrowTimerHandle, this, &AArrow::OnDestroyedArrow, DelayToDestroy, false);
}

void AArrow::OnDestroyedArrow()
{
	GetWorld()->GetTimerManager().ClearTimer(DestroyArrowTimerHandle);

	Destroy();
}