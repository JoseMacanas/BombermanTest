// Fill out your copyright notice in the Description page of Project Settings.

#include "BombermanTestGameModeBase.h"
#include "Engine.h"
#include "LevelGrid.h"

// Sets default values
ABombermanTestGameModeBase::ABombermanTestGameModeBase()
{
	PrimaryActorTick.bCanEverTick = true;	
}


// Called every frame
void ABombermanTestGameModeBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//OnGameEnd();
	//if (GEngine)
	//{
	//	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("GameMode TICK"));
	//}
}

bool ABombermanTestGameModeBase::OnGameRestart()
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("GameRestart"));
	}
	
	UWorld* const World = GetWorld();
	if (World)
	{
		TArray<AActor*> FoundActors;
		UGameplayStatics::GetAllActorsOfClass(World, ALevelGrid::StaticClass(), FoundActors);

		if (FoundActors.Num() > 0)
		{
			ALevelGrid* LevelGrid = Cast<ALevelGrid>(FoundActors[0]);
			if (LevelGrid)
			{
				LevelGrid->RestartGame();
			}
		}
	}

	return false;
}


// Called when the game starts or when spawned
void ABombermanTestGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("GameModeBeginPlay"));
	}

	OnGameRestart();
}



