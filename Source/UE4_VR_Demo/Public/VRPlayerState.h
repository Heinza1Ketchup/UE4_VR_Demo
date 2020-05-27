// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "VRPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class UE4_VR_DEMO_API AVRPlayerState : public APlayerState
{
	GENERATED_BODY()
	

public:

	UFUNCTION(BlueprintCallable, Category="PlayerState")
	void AddScore(float ScoreDelta);
};
