// Fill out your copyright notice in the Description page of Project Settings.


#include "ObjectInteractable.h"
#include "HandController.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"

// Sets default values
AObjectInteractable::AObjectInteractable()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MyMesh"));
	MyMesh->SetSimulatePhysics(true);
	RootComponent = MyMesh;


	bHolding = false;
	bGravity = true;
}

// Called when the game starts or when spawned
void AObjectInteractable::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AObjectInteractable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AObjectInteractable::PickUp()
{

	//bHolding = true;
	//bGravity = false;
	MyMesh->SetEnableGravity(false);
	MyMesh->SetSimulatePhysics(false);
	MyMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

}
void AObjectInteractable::Drop()
{
	MyMesh->SetEnableGravity(true);
	MyMesh->SetSimulatePhysics(true);
	MyMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void AObjectInteractable::Interact()
{

}