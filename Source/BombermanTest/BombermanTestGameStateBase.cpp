// Fill out your copyright notice in the Description page of Project Settings.

#include "BombermanTestGameStateBase.h"

#include "Engine.h"



// Sets default values
ABombermanTestGameStateBase::ABombermanTestGameStateBase()
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("GameStateStart"));
	}	
}

