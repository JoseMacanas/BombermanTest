// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pickups/Pickup.h"
#include "PickupFire.generated.h"

/**
 * 
 */
UCLASS()
class BOMBERMANTEST_API APickupFire : public APickup
{
	GENERATED_BODY()	
	
	virtual void ApplyEffects(ABomberPawn* Player) override;	
};
