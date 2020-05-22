// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "Ballistic.h"
#include "UE4_VR_Demo.h"
#include "CollisionQueryParams.h"
#include "Engine/EngineTypes.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"

AWeapon::AWeapon()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	FireStartingPoint = CreateDefaultSubobject<USceneComponent>(TEXT("FireStartPoint"));
	FireStartingPoint->SetupAttachment(RootComponent);

	UGameplayStatics::SpawnSoundAttached(WFireSoundCue, RootComponent);
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

}

void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWeapon::WeaponFire()
{
	FHitResult HitResult;

	FVector Start = FireStartingPoint->GetComponentLocation();
	FVector Look = FireStartingPoint->GetForwardVector();
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

		UGameplayStatics::PlaySoundAtLocation(this, WFireSoundCue, GetActorLocation());

		UWorld* World = GetWorld();
		if (World) {
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = GetInstigator();
			// Spawn the projectile at the muzzle.
			ABallistic* Projectile = World->SpawnActor<ABallistic>(ProjectileClass, Start, FireStartingPoint->GetComponentRotation(), SpawnParams);
			if (Projectile)
			{
				// Set the projectile's initial trajectory.
				FVector LaunchDirection = Look;
				Projectile->FireInDirection(LaunchDirection);
			}
		}
		
	}
}
