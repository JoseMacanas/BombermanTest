// Fill out your copyright notice in the Description page of Project Settings.

#include "Bomb.h"
#include "LevelGrid.h"
#include "Player/BomberPawn.h"


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

	if (bPlacedInWorld)
	{
		ExplodeTimer -= DeltaTime;
		if (ExplodeTimer <= 0)
		{
			Explode();
		}
	}
}


// Place in the grid
bool ABomb::PlaceInWorld(ALevelGrid* OccupiedLevelGrid, FIntPoint Cell)
{
	if (OccupiedLevelGrid)
	{
		CurrentLevelGrid = OccupiedLevelGrid;
		SetCurrentCell(Cell);
		
		if (CurrentLevelGrid->IsCellWalkable(CurrentCell))
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


// Spawn a chain reaction
void ABomb::Explode()
{	
	if (CurrentLevelGrid)
	{
		CurrentLevelGrid->SpawnChainReaction(this);
	}	
}


bool ABomb::OnDamaged()
{
	if (bPlacedInWorld)
	{
		Explode();
		return true;
	}
	return false;
}

// Hide the Bomb but don't destroy it, because it will keep existing in the owner BomberPawn's Bomb pool
bool ABomb::RemoveFromGame()
{
	if (CurrentLevelGrid)
	{
		CurrentLevelGrid->ExitCell(this, CurrentCell);
	}

	bPlacedInWorld = false;
	SetActorHiddenInGame(true);
	SetActorTickEnabled(false);
	SetActorEnableCollision(false);

	if (OwnerPlayer)
	{
		OwnerPlayer->ReleaseBomb();
	}

	return true;
}

bool ABomb::IsWalkable() const
{
	return false;
}

void ABomb::SetCurrentCell(FIntPoint Cell)
{
	CurrentCell = Cell;
}

const FIntPoint ABomb::GetCurrentCell() const
{
	return CurrentCell;
}