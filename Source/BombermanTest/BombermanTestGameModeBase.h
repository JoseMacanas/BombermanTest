// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BombermanTestGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class BOMBERMANTEST_API ABombermanTestGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	ABombermanTestGameModeBase();
	virtual void Tick(float DeltaTime) override;

	

	UFUNCTION(BlueprintImplementableEvent)
	bool OnGameEnd();

	UFUNCTION(BlueprintCallable)
	bool OnGameRestart();
};
