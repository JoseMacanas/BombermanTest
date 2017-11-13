// Fill out your copyright notice in the Description page of Project Settings.

#include "BombermanTestGameModeBase.h"
#include "BombermanTestGameStateBase.h"
#include "LevelGrid.h"
#include "Engine.h"


// Sets default values
ABombermanTestGameModeBase::ABombermanTestGameModeBase()
{
	PrimaryActorTick.bCanEverTick = true;
}


// Called every frame
void ABombermanTestGameModeBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CurrentGameState)
	{
		if (RoundTimer > 0)
		{
			RoundTimer -= DeltaTime;

			if (RoundTimer <= 0)
			{
				AddScoresAndEndGame(true);
			}
		}
	}
}


// Gathers the players' scores and the winners of the round to call the Blueprint function OnGameEnd()
void ABombermanTestGameModeBase::AddScoresAndEndGame(bool bIsTimeOut /*= false*/)
{
	TArray<int> RoundWinners;
	TArray<int> Scores;

	if (CurrentGameState)
	{
		if (!bIsTimeOut)
		{
			CurrentGameState->AddScores();
			RoundWinners = CurrentGameState->LivingPlayers;
		}
		Scores = CurrentGameState->Scores;
	}

	OnGameEnd(PlayerNames, Scores, RoundWinners);
}


// Callback from UI to restart the game. Also called from code for the first round
bool ABombermanTestGameModeBase::OnGameRestart()
{
	//if (GEngine)
	//{
	//	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("GameRestart"));
	//}

	RoundTimer = RoundTimeSeconds;
	if (CurrentLevelGrid)
	{
		CurrentLevelGrid->RestartGame();

		if (CurrentGameState)
		{
			PlayerNames = CurrentLevelGrid->GetPlayerNames();
			CurrentGameState->NewRound(PlayerNames.Num());

			return true;
		}
	}

	return false;
}


// Called when the game starts or when spawned
void ABombermanTestGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	UWorld* const World = GetWorld();
	if (World)
	{
		CurrentGameState = World->GetGameState<ABombermanTestGameStateBase>();

		TArray<AActor*> FoundActors;
		UGameplayStatics::GetAllActorsOfClass(World, ALevelGrid::StaticClass(), FoundActors);

		if (FoundActors.Num() > 0)
		{
			CurrentLevelGrid = Cast<ALevelGrid>(FoundActors[0]);
		}
	}
		
	//if (GEngine)
	//{
	//	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("GameModeBeginPlay"));
	//}

	OnGameRestart();
}



