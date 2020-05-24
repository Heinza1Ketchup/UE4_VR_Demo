// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ActorCharacter.generated.h"

class UCameraComponent;
class USHealthComponent;
class ASWeapon;

UCLASS()
class UE4_VR_DEMO_API AActorCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AActorCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCameraComponent* CameraComp;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	USHealthComponent* HealthComp;

	FTimerHandle TimerHandle_Destroytimer;

	UPROPERTY(BlueprintReadOnly, Category = "PawnCharacter")
	bool bDied;

	void DestroyPawn();

	UFUNCTION()
	void HandleTakeDamage(USHealthComponent* OwningHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	//dynamic material to pulse on damage
	UMaterialInstanceDynamic* MatInst;

	bool bCanMove;

	UPROPERTY(EditDefaultsOnly, Category = "Player")
	TSubclassOf<ASWeapon> StarterWeaponClass;

	UPROPERTY(VisibleDefaultsOnly, Category = "Player")
	FName WeaponAttachSocketName;

	ASWeapon* CurrentWeapon;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	virtual FVector GetPawnViewLocation() const override;
};
