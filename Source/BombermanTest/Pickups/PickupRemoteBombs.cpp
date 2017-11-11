// Fill out your copyright notice in the Description page of Project Settings.

#include "PickupRemoteBombs.h"


#include "Player/BomberPawn.h"

void APickupRemoteBombs::ApplyEffects(ABomberPawn* Player)
{
	Player->IncreaseRemoteBombsTime(TimeIncrease);
}