// Fill out your copyright notice in the Description page of Project Settings.

#include "LevelGrid.h"
#include "CellOccupantInterface.h"
#include "Block.h"
#include "Runtime/Engine/Classes/Engine/World.h"


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
	
	GenerateLevel();
}


// Called every frame
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
	TSet<ICellOccupantInterface*> CellSet = CellOccupants[Cell];
	if (!CellSet.Contains(CellOccuppant))
	{
		CellSet.Add(CellOccuppant);
	}
}

void ALevelGrid::ExitCell(ICellOccupantInterface* CellOccuppant, FIntPoint Cell)
{
	if (CellOccupants.Contains(Cell))
	{
		TSet<ICellOccupantInterface*> CellSet = CellOccupants[Cell];
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

FIntPoint ALevelGrid::GetCellFromWorldCoordinates(FVector2D WorldCoordinates)
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

FVector2D ALevelGrid::GetWorldCoordinatesFromCell(FIntPoint Cell)
{
	FVector2D WorldCoordinates = FVector2D::ZeroVector;

	FVector GridPosition = GetActorLocation();
	FVector2D GridPosition2D = FVector2D(GridPosition.X, GridPosition.Y);

	WorldCoordinates = FVector2D(Cell)*CellSize + GridPosition2D;

	return WorldCoordinates;
}

