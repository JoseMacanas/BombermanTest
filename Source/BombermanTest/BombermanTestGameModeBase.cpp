// Fill out your copyright notice in the Description page of Project Settings.

#include "BombermanTestGameModeBase.h"
#include "Engine.h"
#include "LevelGrid.h"

#include "BombermanTestGameStateBase.h"

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
		}
		if (RoundTimer <= 0)
		{
			AddScoresAndEndGame();
		}
	}
}

void ABombermanTestGameModeBase::AddScoresAndEndGame()
{
	TArray<int> RoundWinners;
	TArray<int> Scores;

	if (CurrentGameState)
	{
		CurrentGameState->AddScores();
		RoundWinners = CurrentGameState->LivingPlayers;
		Scores = CurrentGameState->Scores;
	}

	OnGameEnd(PlayerNames, Scores, RoundWinners);
}



bool ABombermanTestGameModeBase::OnGameRestart()
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("GameRestart"));
	}

	RoundTimer = RoundTimeSeconds * 1000;
	if (CurrentLevelGrid)
	{
		CurrentLevelGrid->RestartGame();

		if (CurrentGameState)
		{
			PlayerNames = CurrentLevelGrid->GetPlayerNames();
			CurrentGameState->NewRound(PlayerNames.Num());
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
		
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("GameModeBeginPlay"));
	}

	OnGameRestart();
}



