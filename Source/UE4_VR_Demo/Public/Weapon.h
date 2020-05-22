// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ObjectInteractable.h"
#include "Weapon.generated.h"

class USoundCue;
class ABallistic;

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

	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	TSubclassOf<class ABallistic> ProjectileClass;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void WeaponFire();

	float BulletRange = 9999.f;
};
