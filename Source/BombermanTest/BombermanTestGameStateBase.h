// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "BombermanTestGameStateBase.generated.h"

/**
 * 
 */
UCLASS()
class BOMBERMANTEST_API ABombermanTestGameStateBase : public AGameStateBase
{
	GENERATED_BODY()
	ABombermanTestGameStateBase();

public:
	void OnPlayerDeath(const TArray<int>& DeadPlayers);
	bool IsGameOver();
	void NewRound(int NumberOfPlayers);
	void AddScores();
	
	TArray<int> LivingPlayers;
	TArray<int> Scores;
};
