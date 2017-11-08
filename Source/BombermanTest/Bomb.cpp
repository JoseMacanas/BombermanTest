// Fill out your copyright notice in the Description page of Project Settings.

#include "Bomb.h"
#include "LevelGrid.h"


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

bool ABomb::PlaceInWorld(ALevelGrid* OccupiedLevelGrid, FIntPoint Cell)
{
	if (OccupiedLevelGrid)
	{
		CurrentLevelGrid = OccupiedLevelGrid;
		CurrentCell = Cell;
		
		if (CurrentLevelGrid->CanPlaceBomb(CurrentCell))
		{
			ExplodeTimer = ExplodeDelay;
			bPlacedInWorld = true;
			
			FVector2D BombPosition2D = CurrentLevelGrid->GetWorldCoordinatesFromCell(CurrentCell);
			FVector BombPosition = FVector(BombPosition2D.X, BombPosition2D.Y, CurrentLevelGrid->GetActorLocation().Z);

			SetActorLocation(BombPosition);
			SetActorHiddenInGame(false);
			SetActorEnableCollision(true);
			SetActorTickEnabled(true);

			CurrentLevelGrid->EnterCell(this, CurrentCell);
			return true;
		}
	}

	return false;
}

void ABomb::Explode()
{
	if (CurrentLevelGrid)
	{
		CurrentLevelGrid->ExitCell(this, CurrentCell);
	}

	bPlacedInWorld = false;
	SetActorHiddenInGame(true);
	SetActorTickEnabled(false);
	SetActorEnableCollision(false);


	CurrentLevelGrid = NULL;
}


bool ABomb::OnDamaged()
{
	if (CurrentLevelGrid)
	{

	}

	bPlacedInWorld = false;
	SetActorHiddenInGame(true);
	SetActorTickEnabled(false);
	SetActorEnableCollision(false);


	CurrentLevelGrid = NULL;
	return true;
}
