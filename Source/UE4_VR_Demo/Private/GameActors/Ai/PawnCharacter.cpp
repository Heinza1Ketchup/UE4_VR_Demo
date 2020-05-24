// Fill out your copyright notice in the Description page of Project Settings.


#include "GameActors/Ai/PawnCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/ActorComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "GameFramework/Character.h"
#include "GameFramework/Actor.h"
#include "DrawDebugHelpers.h"
#include "Character/SHealthComponent.h"
#include "Character/VRCharacter.h"
#include "Sound/SoundCue.h"


// Sets default values
APawnCharacter::APawnCharacter()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetCanEverAffectNavigation(false);
	MeshComp->SetSimulatePhysics(true);
	MeshComp->SetupAttachment(RootComponent);


	HealthComp = CreateDefaultSubobject<USHealthComponent>(TEXT("HealthComp"));
	HealthComp->OnHealthChanged.AddDynamic(this, &APawnCharacter::HandleTakeDamage);

	bUseVelocityChange = false;
	MovementForce = 10.f;

	RequiredDistanceToTarget = 100;

	bDied = false;
}

// Called when the game starts or when spawned
void APawnCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	//find initial move to
	NextPathPoint = GetNextPathPoint();
}

// Called every frame
void APawnCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	
	float DistanceToTarget = (GetActorLocation() - NextPathPoint).Size();

	if(bDied){ return; }

	if (DistanceToTarget <= RequiredDistanceToTarget) {
		NextPathPoint = GetNextPathPoint();

		//DrawDebugString(GetWorld(), GetActorLocation(), "Target Reached!");
	}
	else {
		//keep moving towards next target
		FVector ForceDirection = NextPathPoint - GetActorLocation();
		ForceDirection.Normalize();
		ForceDirection *= MovementForce;
		
		//MeshComp->AddForce(ForceDirection, NAME_None, bUseVelocityChange);
		AddActorLocalOffset(ForceDirection);


		DrawDebugDirectionalArrow(GetWorld(), GetActorLocation(), GetActorLocation() + ForceDirection, 32, FColor::Yellow, false, 0.0f, 0, 1.0f);
	}

	DrawDebugSphere(GetWorld(), NextPathPoint, 20, 12, FColor::Yellow, false, 0.0f, 1.0f);
	
}

FVector APawnCharacter::GetNextPathPoint()
{
	// hack, to get player location
	ACharacter* PlayerPawn = UGameplayStatics::GetPlayerCharacter(this, 0);
	UNavigationPath* NavPath = UNavigationSystemV1::FindPathToActorSynchronously(this, GetActorLocation(), PlayerPawn);
	if (NavPath && NavPath->PathPoints.Num() > 1) {
		//return next point in path
		return NavPath->PathPoints[1];
	}

	//failed to find path
	return GetActorLocation();
}

void APawnCharacter::HandleTakeDamage(USHealthComponent* OwningHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{

	if (MatInst == nullptr) {
		MatInst = MeshComp->CreateAndSetMaterialInstanceDynamicFromMaterial(0, MeshComp->GetMaterial(0));
	}

	if (MatInst) {
		MatInst->SetScalarParameterValue("LastTimeDamageTaken", GetWorld()->TimeSeconds);
	}


	UE_LOG(LogTemp, Log, TEXT("Health %s of %s"), *FString::SanitizeFloat(Health), *GetName());

	if (Health <= 0.0f) {
	//
		bDied = true;
		GetWorldTimerManager().SetTimer(TimerHandle_Destroytimer, this, &APawnCharacter::DestroyPawn, 4, false);
	}

}

void APawnCharacter::DestroyPawn()
{
	Destroy();
}

void  APawnCharacter::NotifyActorBeginOverlap(AActor* OtherActor)
{
	
}
