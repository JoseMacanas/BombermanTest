// Fill out your copyright notice in the Description page of Project Settings.

#include "BombermanTestGameStateBase.h"
#include "BombermanTestGameModeBase.h"
#include "Engine.h"


// Sets default values
ABombermanTestGameStateBase::ABombermanTestGameStateBase()
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("GameStateStart"));
	}	
}


// Reset living players and set the scores to 0 if not set yet
void ABombermanTestGameStateBase::NewRound(int NumberOfPlayers)
{
	LivingPlayers.Empty();

	for (int PlayerIndex = 0; PlayerIndex < NumberOfPlayers; ++PlayerIndex)
	{
		LivingPlayers.Add(PlayerIndex);
		if (Scores.Num() <= PlayerIndex)
		{
			Scores.Add(0);
		}
	}	
}


// Called when one of more players dies. The reason this is not called by the players themselves is 
// to take into account the death of multiple players in a single chain reaction
void ABombermanTestGameStateBase::OnPlayerDeath(const TArray<int>& DeadPlayers)
{
	for (int PlayerIterator = 0; PlayerIterator < DeadPlayers.Num(); ++PlayerIterator)
	{
		int DeadPlayerId = DeadPlayers[PlayerIterator];
		if (LivingPlayers.Contains(DeadPlayerId))
		{
			LivingPlayers.Remove(DeadPlayerId);
		}
	}

	if (LivingPlayers.Num() < 2)
	{
		UWorld* const World = GetWorld();
		if (World)
		{					
			ABombermanTestGameModeBase* GameMode = World->GetAuthGameMode<ABombermanTestGameModeBase>();

			if (GameMode)
			{
				GameMode->AddScoresAndEndGame();
			}
		}
	}
}


// Add a point to every living player
void ABombermanTestGameStateBase::AddScores()
{
	for (int LivingPlayerIndex = 0; LivingPlayerIndex < LivingPlayers.Num(); ++LivingPlayerIndex)
	{
		int PlayerIndex = LivingPlayers[LivingPlayerIndex];
		if (Scores.Num() > PlayerIndex)
		{
			Scores[PlayerIndex] ++;
		}
	}
}

