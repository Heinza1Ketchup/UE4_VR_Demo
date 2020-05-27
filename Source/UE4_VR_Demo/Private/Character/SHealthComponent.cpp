// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/SHealthComponent.h"
#include "Net/UnrealNetwork.h"
#include "SGameMode.h"
#include "Components/ActorComponent.h"

// Sets default values for this component's properties
USHealthComponent::USHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	DefaultHealth = 100;
	TeamNum = 255;
}


// Called when the game starts
void USHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	bIsDead = false;

	

	AActor* MyOwner = GetOwner();
	if (MyOwner)
	{
		MyOwner->OnTakeAnyDamage.AddDynamic(this, &USHealthComponent::HandleTakeAnyDamage);


	}

	Health = DefaultHealth;
}

void USHealthComponent::HandleTakeAnyDamage(AActor * DamagedActor, float Damage, const UDamageType * DamageType, AController * InstigatedBy, AActor * DamageCauser)
{

	
	if (Damage <= 0.0f || bIsDead) {
		return;
	}

	//DamageCauser != DamagedActor &&
	//if ( IsFriendly(GetOwner(), DamageCauser)) {return;}


	//update health clamp
	Health = FMath::Clamp(Health - Damage, 0.0f, DefaultHealth);

	//UE_LOG(LogTemp, Log, TEXT("Health Changed: %s"), *FString::SanitizeFloat(Health))

	bIsDead = (Health <= 0.0f);

	OnHealthChanged.Broadcast(this, Health, Damage, DamageType, InstigatedBy, DamageCauser);

	if (Health <= 0.0f) {
		ASGameMode* GM = Cast<ASGameMode>(GetWorld()->GetAuthGameMode());
		if (GM) {
			GM->OnActorKilled.Broadcast(GetOwner(), DamageCauser, InstigatedBy);
		}
	}
	
}

void USHealthComponent::Heal(float Value)
{
	if (Health <= 0.0f || Value <= 0.0f) {
		return;
	}
	Health = FMath::Clamp(Health + Value, 0.0f, DefaultHealth);

	UE_LOG(LogTemp, Log, TEXT("Health Changed: %s (+%s)"), *FString::SanitizeFloat(Health), *FString::SanitizeFloat(Value))

		OnHealthChanged.Broadcast(this, Health, -Value, nullptr, nullptr, nullptr);
}

bool USHealthComponent::IsFriendly(AActor * ActorA, AActor * ActorB)
{
	if (ActorA == nullptr || ActorB == nullptr) {
		return false;
	}

	USHealthComponent* HealthCompA = Cast<USHealthComponent>(ActorA->GetComponentByClass(USHealthComponent::StaticClass()));
	USHealthComponent* HealthCompB = Cast<USHealthComponent>(ActorB->GetComponentByClass(USHealthComponent::StaticClass()));

	if (HealthCompB == nullptr || HealthCompA == nullptr) {
		return true;
	}

	return HealthCompA->TeamNum == HealthCompB->TeamNum;
}

float USHealthComponent::GetHealth() const 
{
	return Health;
}