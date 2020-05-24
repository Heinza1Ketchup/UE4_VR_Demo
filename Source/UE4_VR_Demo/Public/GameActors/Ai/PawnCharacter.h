// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PawnCharacter.generated.h"

class USHealthComponent;
class UCapsuleComponent;

UCLASS()
class UE4_VR_DEMO_API APawnCharacter : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	APawnCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	USceneComponent* MySceneComp;

	UPROPERTY(VisibleDefaultsOnly, Category="Components")
	USkeletalMeshComponent* MeshComp;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	USHealthComponent* HealthComp;

	FTimerHandle TimerHandle_Destroytimer;

	FVector GetNextPathPoint();

	//Next Point in Navigation path
	FVector NextPathPoint;

	UPROPERTY(EditDefaultsOnly, Category = "PawnCharacter")
	float MovementForce;

	UPROPERTY(EditDefaultsOnly, Category = "PawnCharacter")
	bool bUseVelocityChange;

	UPROPERTY(EditDefaultsOnly, Category = "PawnCharacter")
	float RequiredDistanceToTarget;

	//dynamic material to pulse on damage
	UMaterialInstanceDynamic* MatInst;

	//pawn died previously
	UPROPERTY(BlueprintReadOnly, Category = "PawnCharacter")
	bool bDied;

	void DestroyPawn();

	UFUNCTION()
	void HandleTakeDamage(USHealthComponent* OwningHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

};
