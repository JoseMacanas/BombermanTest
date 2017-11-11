// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pickups/Pickup.h"
#include "PickupBombs.generated.h"

/**
 * 
 */
UCLASS()
class BOMBERMANTEST_API APickupBombs : public APickup
{
	GENERATED_BODY()
	
	virtual void ApplyEffects(ABomberPawn* Player) override;	
};
