// Fill out your copyright notice in the Description page of Project Settings.


#include "GameActors/Weapon.h"
#include "GameActors/Ballistic.h"
#include "UE4_VR_Demo.h"
#include "CollisionQueryParams.h"
#include "Engine/EngineTypes.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Particles/ParticleSystemComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "TimerManager.h"
#include "Net/UnrealNetwork.h"

AWeapon::AWeapon()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	FireStartingPoint = CreateDefaultSubobject<USceneComponent>(TEXT("FireStartPoint"));
	FireStartingPoint->SetupAttachment(MyMesh);

	UGameplayStatics::SpawnSoundAttached(WFireSoundCue, RootComponent);
	UGameplayStatics::SpawnSoundAttached(WReloadSoundCue, RootComponent);

	MuzzleSocketName = "MuzzleSocket";
	TracerTargetName = "Target";
	BaseDamage = 20.f;
	RateOfFire = 720;
	RoundsInClip = 30;

}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	LastFireTime = 0;
	TimeBetweenShots = 60 / RateOfFire;
}

void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWeapon::WeaponFire()
{
	if (RoundsInClip > 0) {
		FVector Start = FireStartingPoint->GetComponentLocation();
		FVector Look = FireStartingPoint->GetForwardVector();
		FVector End = Start + Look * BulletRange;

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(this);
		QueryParams.bTraceComplex = true;
		QueryParams.bReturnPhysicalMaterial = true;

		FVector TracerEndPoint = End;

		FHitResult HitResult;
		bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, COLLISION_WEAPON, QueryParams);

		UGameplayStatics::PlaySoundAtLocation(this, WFireSoundCue, GetActorLocation());

		UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, FireStartingPoint);

		//spawn physical projectile
		/*UWorld* World = GetWorld();
			if (World) {
				FActorSpawnParameters SpawnParams;
				SpawnParams.Owner = this;
				SpawnParams.Instigator = GetInstigator();
				// Spawn the projectile at the muzzle.
				ABallistic* Projectile = World->SpawnActor<ABallistic>(ProjectileClass, Start, FireStartingPoint->GetComponentRotation(), SpawnParams);
				if (Projectile){
					// Set the projectile's initial trajectory.
					FVector LaunchDirection = Look;
					Projectile->FireInDirection(LaunchDirection);
				}

			}*/

		if (bHit) {
			AActor* Hit = HitResult.GetActor();
			EPhysicalSurface SufraceType = UPhysicalMaterial::DetermineSurfaceType(HitResult.PhysMaterial.Get());

			//DrawDebugLine(GetWorld(), Start, End, FColor::White, false, 1.0f, 0, 1.0f);

			EPhysicalSurface SurfaceType = UPhysicalMaterial::DetermineSurfaceType(HitResult.PhysMaterial.Get());

			float ActualDamage = BaseDamage;
			if (SurfaceType == SURFACE_FLESHVULNERABLE) { ActualDamage = 45.f; }
			if (SufraceType == SURFACE_FLESHLEATHAL) { ActualDamage = 100.f; }

			UGameplayStatics::ApplyPointDamage(Hit, ActualDamage, Look, HitResult, nullptr, this, DamageType);

			UParticleSystem* SelectedEffect = nullptr;
			switch (SufraceType) {
			case SURFACE_FLESHVULNERABLE:
				SelectedEffect = FleshImpactEffect;
				break;
			case SURFACE_FLESHLEATHAL:
				SelectedEffect = FleshImpactEffect;
				break;
			default:
				SelectedEffect = DefaultImpactEffect;
				break;
			}
			if (SelectedEffect) {
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SelectedEffect, HitResult.ImpactPoint, HitResult.ImpactNormal.Rotation());
			}

			TracerEndPoint = HitResult.ImpactPoint;
			if (TracerEffect) {
				//FVector MuzzleLocation = FireStartingPoint->GetSocketLocation(MuzzleSocketName);

				UParticleSystemComponent* TracerComp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TracerEffect, FireStartingPoint->GetComponentLocation());

				if (TracerComp) {
					TracerComp->SetVectorParameter(TracerTargetName, TracerEndPoint);
				}
			}

			LastFireTime = GetWorld()->TimeSeconds;
		}

		RoundsInClip--;
	}

	
}

void AWeapon::StartFire()
{
	if (LastFireTime == 0) {
		WeaponFire();
	}
	float FirstDelay = FMath::Max(LastFireTime + TimeBetweenShots - GetWorld()->TimeSeconds, 0.0f);

	GetWorldTimerManager().SetTimer(TimerHandle_TimeBetweenShots, this, &AWeapon::WeaponFire, TimeBetweenShots, FirstDelay);
	
	
}
void AWeapon::StopFire()
{
	LastFireTime = 0;
	GetWorldTimerManager().ClearTimer(TimerHandle_TimeBetweenShots);
}

void AWeapon::ReloadWeapon()
{
	UGameplayStatics::PlaySoundAtLocation(this, WReloadSoundCue, GetActorLocation());
	RoundsInClip = 30;
}
