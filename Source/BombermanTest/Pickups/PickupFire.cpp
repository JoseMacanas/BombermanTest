// Fill out your copyright notice in the Description page of Project Settings.

#include "PickupFire.h"
#include "Player/BomberPawn.h"


void APickupFire::ApplyEffects(ABomberPawn* Player)
{
	Player->IncreaseExplosionRange(1);
}
