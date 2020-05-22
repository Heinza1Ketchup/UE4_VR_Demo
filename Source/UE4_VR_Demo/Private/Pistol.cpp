// Fill out your copyright notice in the Description page of Project Settings.


#include "Pistol.h"
#include "Components/StaticMeshComponent.h"
#include "MotionControllerComponent.h"
#include "GameFramework/Character.h"

APistol::APistol()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MyMesh"));
	MyMesh->SetSimulatePhysics(true);
	RootComponent = MyMesh;

	FireStartingPoint = CreateDefaultSubobject<USceneComponent>(TEXT("FireStartPoint"));
	FireStartingPoint->SetupAttachment(RootComponent);
}

void APistol::BeginPlay()
{
	Super::BeginPlay();

}

void APistol::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}