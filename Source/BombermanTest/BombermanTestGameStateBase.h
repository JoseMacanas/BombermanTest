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
	// Called when one of more players dies. The reason this is not called by the players themselves is 
	// to take into account the death of multiple players in a single chain reaction
	void OnPlayerDeath(const TArray<int>& DeadPlayers);

	// Reset living players and set the scores to 0 if not set yet
	void NewRound(int NumberOfPlayers);

	// Add a point to every living player
	void AddScores();
	
	TArray<int> LivingPlayers;
	TArray<int> Scores;
};
