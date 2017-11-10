// Fill out your copyright notice in the Description page of Project Settings.

#include "LevelGrid.h"
#include "CellOccupantInterface.h"
#include "Block.h"
#include "Bomb.h"
#include "Explosion.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "Engine.h"


// Sets default values
ALevelGrid::ALevelGrid()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ALevelGrid::BeginPlay()
{
	Super::BeginPlay();
}

void ALevelGrid::RestartGame()
{
	for (auto CellOccupantIterator = CellOccupants.CreateConstIterator(); CellOccupantIterator; ++CellOccupantIterator)
	{
		TSet<ICellOccupantInterface*> CellSet = CellOccupantIterator.Value();

		for (auto CellSetIterator = CellSet.CreateConstIterator(); CellSetIterator; ++CellSetIterator)
		{
			ICellOccupantInterface* CellOccupant = *CellSetIterator;
			CellOccupant->RemoveFromGame();
		}
	}

	GenerateLevel();

	PlacePlayers();
}


void ALevelGrid::PlacePlayers()
{
	// The grid real valid positions go from (1,1) to (GridWidth, GridHeight), to take into account the borders of the grid
	// Place up to four players in the corners.
	TArray<FIntPoint> StartingPositions;
	StartingPositions.Add(FIntPoint(1, 1));
	StartingPositions.Add(FIntPoint(GridWidth, GridHeight));
	StartingPositions.Add(FIntPoint(1, GridHeight));
	StartingPositions.Add(FIntPoint(GridWidth, 1));

	for (int PlayerIndex = 0; PlayerIndex < Players.Num(); ++PlayerIndex)
	{
		ABomberPawn* Player = Players[PlayerIndex];
		if (Player)
		{
			Player->PlaceInGrid(this, StartingPositions[PlayerIndex]);
		}
	}
}


void ALevelGrid::GenerateLevel(int RandomSeed)
{
	FRandomStream RandomNumberGenerator;
	RandomNumberGenerator.Initialize(RandomSeed);
	
	UWorld* const World = GetWorld();
	if (World && BorderBlockBPClass && SolidBlockBPClass)
	{
		for (int RowIndex = 0; RowIndex < GridHeight + 2; ++RowIndex)
		{
			for (int ColumnIndex = 0; ColumnIndex < GridWidth + 2; ++ColumnIndex)
			{
				FIntPoint Cell = FIntPoint(RowIndex, ColumnIndex);
				FVector2D BlockPosition = GetWorldCoordinatesFromCell(Cell);

				if (ColumnIndex == 0 || RowIndex == 0 || ColumnIndex == GridWidth + 1 || RowIndex == GridHeight + 1)
				{
					FActorSpawnParameters SpawnParams;
					ABlock* Block = World->SpawnActor<ABlock>(BorderBlockBPClass, FVector(BlockPosition.X, BlockPosition.Y, GetActorLocation().Z), FRotator::ZeroRotator, SpawnParams);

					EnterCell(Block, Cell);
				}
				else if (RowIndex % 2 == 0 && ColumnIndex % 2 == 0)
				{
					FActorSpawnParameters SpawnParams;
					ABlock* Block = World->SpawnActor<ABlock>(SolidBlockBPClass, FVector(BlockPosition.X, BlockPosition.Y, GetActorLocation().Z), FRotator::ZeroRotator, SpawnParams);

					EnterCell(Block, Cell);
				}
			}
		}
	}
}


// Called every frame
void ALevelGrid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ALevelGrid::EnterCell(ICellOccupantInterface* CellOccuppant, FIntPoint Cell)
{
	if (!CellOccupants.Contains(Cell))
	{
		CellOccupants.Add(Cell, TSet<ICellOccupantInterface*>()); 
	}
	TSet<ICellOccupantInterface*>& CellSet = CellOccupants[Cell];
	if (!CellSet.Contains(CellOccuppant))
	{
		CellSet.Add(CellOccuppant);
	}

	bool bICellOnFire = false;
	for (auto& Elem : CellSet)
	{
		if (Cast<AExplosion>(Elem))
		{
			bICellOnFire = true;
			break;
		}
	}

	if (bICellOnFire)
	{
		for (auto& Elem : CellSet)
		{
			Elem->OnDamaged();
		}
	}
}

void ALevelGrid::ExitCell(ICellOccupantInterface* CellOccuppant, FIntPoint Cell)
{
	if (CellOccupants.Contains(Cell))
	{
		TSet<ICellOccupantInterface*>& CellSet = CellOccupants[Cell];
		if (CellSet.Contains(CellOccuppant))
		{
			CellSet.Remove(CellOccuppant);
		}
	}
}

void ALevelGrid::ChangeCell(ICellOccupantInterface* CellOccuppant, FIntPoint OldCell, FIntPoint NewCell)
{
	ExitCell(CellOccuppant, OldCell);
	EnterCell(CellOccuppant, NewCell);
}

FIntPoint ALevelGrid::GetCellFromWorldCoordinates(FVector2D WorldCoordinates) const
{
	FIntPoint Cell = FIntPoint::ZeroValue;

	if (CellSize > 0)
	{
		FVector GridPosition = GetActorLocation();
		FVector2D GridPosition2D = FVector2D(GridPosition.X, GridPosition.Y);

		FVector2D CellPosition = (WorldCoordinates - GridPosition2D) / CellSize;
		
		Cell = CellPosition.IntPoint();	
	}

	return Cell;
}

FVector2D ALevelGrid::GetWorldCoordinatesFromCell(FIntPoint Cell) const
{
	FVector2D WorldCoordinates = FVector2D::ZeroVector;

	FVector GridPosition = GetActorLocation();
	FVector2D GridPosition2D = FVector2D(GridPosition.X, GridPosition.Y);

	WorldCoordinates = FVector2D(Cell)*CellSize + GridPosition2D;

	return WorldCoordinates;
}


bool ALevelGrid::IsCellWalkable(FIntPoint Cell) const
{
	if (CellOccupants.Contains(Cell))
	{
		TSet<ICellOccupantInterface*> CellSet = CellOccupants[Cell];

		for (auto& Elem : CellSet)
		{
			if (Cast<ABomb>(Elem) || Cast<ABlock>(Elem))
			{
				return false;
			}
		}
	}

	return true;
}

void ALevelGrid::SpawnExplosion(FIntPoint Cell, int ExplosionSize)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, FString::Printf(TEXT("EXPLOSION at %d - %d!"), Cell.X, Cell.Y));
	}

	if (ExplosionBPClass)
	{
		UWorld* const World = GetWorld();
	
		TArray<FIntPoint> ExplosionDirections;
		ExplosionDirections.Add(FIntPoint(0, 1)); // North
		ExplosionDirections.Add(FIntPoint(0, -1)); // South
		ExplosionDirections.Add(FIntPoint(1, 0)); // West
		ExplosionDirections.Add(FIntPoint(-1, 0)); // East

		FActorSpawnParameters SpawnParams;
		FVector2D AffectedCellWorldLocation = GetWorldCoordinatesFromCell(Cell);
		AExplosion* Explosion = World->SpawnActor<AExplosion>(ExplosionBPClass, FVector(AffectedCellWorldLocation.X, AffectedCellWorldLocation.Y, GetActorLocation().Z), FRotator::ZeroRotator, SpawnParams);
		Explosion->CurrentLevelGrid = this;
		Explosion->CurrentCell = Cell;

		for (int ExplosionRadiusIndex = 1; ExplosionRadiusIndex < ExplosionSize; ++ExplosionRadiusIndex)
		{
			for (int DirectionIndex = ExplosionDirections.Num() -1; DirectionIndex >= 0; --DirectionIndex)
			{
				FIntPoint NextAffectedCell = Cell + (ExplosionDirections[DirectionIndex] * ExplosionRadiusIndex);
				FVector2D NextAffectedCellWorldLocation = GetWorldCoordinatesFromCell(NextAffectedCell);
				AExplosion* Explosion = World->SpawnActor<AExplosion>(ExplosionBPClass, FVector(NextAffectedCellWorldLocation.X, NextAffectedCellWorldLocation.Y, GetActorLocation().Z), FRotator::ZeroRotator, SpawnParams);
				Explosion->CurrentLevelGrid = this;
				Explosion->CurrentCell = NextAffectedCell;

				if (!IsCellWalkable(NextAffectedCell))
				{
					ExplosionDirections.RemoveAt(DirectionIndex);
				}
			}
		}
	}
}