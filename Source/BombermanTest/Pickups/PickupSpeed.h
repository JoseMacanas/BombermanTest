// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pickups/Pickup.h"
#include "PickupSpeed.generated.h"

/**
 * 
 */
UCLASS()
class BOMBERMANTEST_API APickupSpeed : public APickup
{
	GENERATED_BODY()
	
	virtual void ApplyEffects(ABomberPawn* Player) override;
	
	UPROPERTY(EditDefaultsOnly)
	float SpeedIncrease = 100.f;
};
