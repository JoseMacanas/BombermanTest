// Fill out your copyright notice in the Description page of Project Settings.

#include "PickupBombs.h"
#include "Player/BomberPawn.h"


void APickupBombs::ApplyEffects(ABomberPawn* Player)
{
	Player->IncreaseAvailableBombs(1);
}
