// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SWeapon.generated.h"

class USkeletalMeshComponent;
class UDamageType;
class UParticleSystem;
class USoundCue;
class AudioComponent;

UCLASS()
class UE4_VR_DEMO_API ASWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		USkeletalMeshComponent* MeshComp;

	void PlayFireEffect(FVector TraceEndPoint);

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
	TSubclassOf<UCameraShake> FireCamShake;

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

	//
	UPROPERTY(EditDefaultsOnly, Category = "TracketBot")
		class USoundCue* RifleShotSoundCue;
	UPROPERTY(EditDefaultsOnly, Category = "TracketBot")
		class USoundCue* RifleShotEmptySoundCue;
	UPROPERTY(EditDefaultsOnly, Category = "TracketBot")
		class USoundCue* RifleReloadSoundCue;

public:	
	
	UFUNCTION()
	void Fire();

	void StartFire();
	void StopFire();
	void ReloadWeapon();

private:
	UAudioComponent* RifleShotAudioComponent;
};
