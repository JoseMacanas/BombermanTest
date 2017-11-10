// Fill out your copyright notice in the Description page of Project Settings.

#include "BombermanTestGameModeBase.h"

#include "Engine.h"

// Sets default values
ABombermanTestGameModeBase::ABombermanTestGameModeBase()
{
	PrimaryActorTick.bCanEverTick = true;

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("GameModeStart"));
	}	
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

	return false;
}



