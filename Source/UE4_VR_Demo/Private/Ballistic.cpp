// Fill out your copyright notice in the Description page of Project Settings.

#include "UE4_VR_Demo.h"
#include "Ballistic.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"

// Sets default values
ABallistic::ABallistic()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Use a sphere as a simple collision representation.
	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	// Set the sphere's collision radius.
	CollisionComponent->InitSphereRadius(15.0f);
	// Set the root component to be the collision component.
	RootComponent = CollisionComponent;

	// Use this component to drive this projectile's movement.
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->SetUpdatedComponent(CollisionComponent);
	ProjectileMovementComponent->InitialSpeed = 9999.0f;
	ProjectileMovementComponent->MaxSpeed = 9999.0f;
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->bShouldBounce = true;
	ProjectileMovementComponent->Bounciness = 0.3f;
}

// Called when the game starts or when spawned
void ABallistic::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABallistic::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABallistic::FireInDirection(const FVector& ShootDirection)
{
	ProjectileMovementComponent->Velocity = ShootDirection * ProjectileMovementComponent->InitialSpeed;
}
