// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ObjectInteractable.h"
#include "Weapon.generated.h"

class USoundCue;
class ABallistic;
class UDamageType;
class UParticleSystem;

/**
 * 
 */
UCLASS()
class UE4_VR_DEMO_API AWeapon : public AObjectInteractable
{
	GENERATED_BODY()
	
public:
	AWeapon();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneComponent* FireStartingPoint;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	USoundCue* WFireSoundCue;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	USoundCue* WReloadSoundCue;

	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	TSubclassOf<class ABallistic> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<UDamageType> DamageType;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FName MuzzleSocketName;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FName TracerTargetName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem* MuzzleEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem* DefaultImpactEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem* FleshImpactEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem* TracerEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float BaseDamage;

	FTimerHandle TimerHandle_TimeBetweenShots;

	float LastFireTime;
	// rpm -rounds per minute
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float RateOfFire;
	//derive from rate of fire
	float TimeBetweenShots;

	float RoundsInClip;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void WeaponFire();

	void StartFire();
	void StopFire();
	void ReloadWeapon();

	float BulletRange = 9999.f;
};
