// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pickups/Pickup.h"
#include "PickupRemoteBombs.generated.h"

/**
 * 
 */
UCLASS()
class BOMBERMANTEST_API APickupRemoteBombs : public APickup
{
	GENERATED_BODY()
		
	virtual void ApplyEffects(ABomberPawn* Player) override;

	UPROPERTY(EditDefaultsOnly)
	float TimeIncrease = 10000.f;
};
