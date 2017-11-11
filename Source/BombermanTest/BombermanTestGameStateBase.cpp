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

bool ABombermanTestGameStateBase::IsGameOver()
{
	bool IsRoundOver = (LivingPlayers.Num() <= 1);
	return false;
}

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

