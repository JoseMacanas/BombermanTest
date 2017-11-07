// Fill out your copyright notice in the Description page of Project Settings.

#include "Bomb.h"


// Sets default values
ABomb::ABomb()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	SetActorTickEnabled(false);
	SetActorEnableCollision(false);
	SetActorHiddenInGame(true);
}

// Called when the game starts or when spawned
void ABomb::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABomb::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ExplodeTimer -= DeltaTime;

	if (ExplodeTimer <= 0)
	{
		Explode();
	}
}

void ABomb::PlaceInWorld(FVector Location)
{
	ExplodeTimer = ExplodeDelay;
	bPlacedInWorld = true;
	SetActorLocation(Location);
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
	SetActorTickEnabled(true);
}

void ABomb::Explode()
{
	bPlacedInWorld = false;
	SetActorHiddenInGame(true);
	SetActorTickEnabled(false);
	SetActorEnableCollision(false);
}

