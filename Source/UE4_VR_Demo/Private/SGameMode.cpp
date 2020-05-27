// Fill out your copyright notice in the Description page of Project Settings.
#include "SGameMode.h"
#include "Character/SHealthComponent.h"
#include "GameFramework/Pawn.h"
#include "SGameStateBase.h"
#include "VRPlayerState.h"
#include "UE4_VR_Demo.h"



ASGameMode::ASGameMode()
{
	//	
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 1.0f;

	GameStateClass = ASGameStateBase::StaticClass();
	PlayerStateClass = AVRPlayerState::StaticClass();

	TimeBetweenWaves = 2.0f;
	WaveCount = 0;

	PlayerStartedRound = false;
}

void ASGameMode::SetWaveState(EWaveState NewState) {
	ASGameStateBase* GS = GetGameState<ASGameStateBase>();
	if (ensureAlways(GS)) {
		GS->SetWaveState(NewState);
	}
}

void ASGameMode::StartWave()
{
	WaveCount++;
	NumofBotsSpawn = 2 * WaveCount;
	GetWorldTimerManager().SetTimer(TimerHandle_BotSpawner, this, &ASGameMode::SpawnBotTimerElapsed, 1.0f, true, 0.0f);

	SetWaveState(EWaveState::WaveInProgress);
}

void ASGameMode::EndWave()
{

	GetWorldTimerManager().ClearTimer(TimerHandle_BotSpawner);

	SetWaveState(EWaveState::WaitingToComplete);
}

void ASGameMode::SpawnBotTimerElapsed()
{
	SpawnNewBot();

	NumofBotsSpawn--;

	if (NumofBotsSpawn <= 0) {
		EndWave();
	}
}

void ASGameMode::PrepareForNextWave()
{
	GetWorldTimerManager().SetTimer(TimerHandle_NextWavetoStart, this, &ASGameMode::StartWave, TimeBetweenWaves, false);

	SetWaveState(EWaveState::WaitingToStart);
}

void ASGameMode::CheckWaveState()
{

	bool bIsPreparingForWave = GetWorldTimerManager().IsTimerActive(TimerHandle_NextWavetoStart);

	if (NumofBotsSpawn > 0 || bIsPreparingForWave) {
		return;
	}

	bool bIsAnyBotAlive = false;

	for (FConstPawnIterator It = GetWorld()->GetPawnIterator(); It; ++It) {
		APawn* TestPawn = It->Get();
		if (TestPawn == nullptr || TestPawn->IsPlayerControlled()) {
			continue;
		}

		USHealthComponent* HealthComp = Cast<USHealthComponent>(TestPawn->GetComponentByClass(USHealthComponent::StaticClass()));
		if (HealthComp && HealthComp->GetHealth() > 0.0f)
		{
			bIsAnyBotAlive = true;
			break;
		}
	}

	if (!bIsAnyBotAlive && PlayerStartedRound) {
		SetWaveState(EWaveState::WaveComplete);

		PrepareForNextWave();
	}
	
}
void ASGameMode::CheckAnyPlayerAlive()
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It) {
		APlayerController* PC = It->Get();
		if (PC && PC->GetPawn()) {
			APawn* MyPawn = PC->GetPawn();
			USHealthComponent* HealthComp = Cast<USHealthComponent>(MyPawn->GetComponentByClass(USHealthComponent::StaticClass()));
			if (ensure(HealthComp) && HealthComp->GetHealth() > 0.0f) {
				return;
			}
		}
	}
	//no player alive
	GameOverState();
	PlayerStartedRound = false;
}

void ASGameMode::GameOverState()
{
	EndWave();
	
	SetWaveState(EWaveState::GameOver);

	UE_LOG(LogTemp, Log, TEXT("Game Over"));
}

void ASGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	CheckWaveState();

	CheckAnyPlayerAlive();
}

void ASGameMode::StartPlay()
{
	Super::StartPlay();

	//PrepareForNextWave();
}

void ASGameMode::StartGameFromPlayer() {
	PlayerStartedRound = true;
}