// Fill out your copyright notice in the Description page of Project Settings.

#include "PickupSpeed.h"
#include "Player/BomberPawn.h"


void APickupSpeed::ApplyEffects(ABomberPawn* Player)
{
	Player->IncreaseSpeed(SpeedIncrease);
}
