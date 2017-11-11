// Fill out your copyright notice in the Description page of Project Settings.

#include "Pickup.h"
#include "LevelGrid.h"

// Sets default values
APickup::APickup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APickup::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


bool APickup::OnDamaged()
{
	RemoveFromGame();
	return true;
}

bool APickup::RemoveFromGame()
{
	if (CurrentLevelGrid)
	{
		CurrentLevelGrid->ExitCell(this, CurrentCell);
	}
	Destroy();
	return true;
}


bool APickup::IsWalkable() const
{
	return true;
}