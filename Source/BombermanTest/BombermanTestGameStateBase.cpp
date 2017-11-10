// Fill out your copyright notice in the Description page of Project Settings.

#include "BombermanTestGameStateBase.h"
#include "BombermanTestGameModeBase.h"

#include "Player/BomberPawn.h"
#include "Engine.h"



// Sets default values
ABombermanTestGameStateBase::ABombermanTestGameStateBase()
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("GameStateStart"));
	}	
}


void ABombermanTestGameStateBase::OnPlayerDeath(ABomberPawn* DeadPlayer)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("PlayerDeathDetected on GameState"));
	}
	
	UWorld* const World = GetWorld();
	if (World)
	{
		ABombermanTestGameModeBase* GameMode = Cast<ABombermanTestGameModeBase>(World->GetAuthGameMode());

		if (GameMode)
		{
			GameMode->OnGameEnd();
		}
	}

}

