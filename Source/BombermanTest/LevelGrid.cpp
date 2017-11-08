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
	
	UWorld* const World = GetWorld();
	if (World)
	{
		for (int RowIndex = 0; RowIndex < GridHeight + 2; ++RowIndex)
		{
			for (int ColumnIndex = 0; ColumnIndex < GridWidth + 2; ++ColumnIndex)
			{
				FVector2D BlockPosition = GetWorldCoordinatesFromCell(FVector2D(RowIndex, ColumnIndex));

				if (ColumnIndex == 0 || RowIndex == 0 || ColumnIndex == GridWidth + 1 || RowIndex == GridHeight + 1)
				{
					if (BorderBlockBPClass)
					{
						FActorSpawnParameters SpawnParams;
						World->SpawnActor<ABlock>(BorderBlockBPClass, FVector(BlockPosition.X, BlockPosition.Y, GetActorLocation().Z), FRotator::ZeroRotator, SpawnParams);
					}
				}
				else if (RowIndex % 2 == 0 && ColumnIndex % 2 == 0)
				{
					if (SolidBlockBPClass)
					{
						FActorSpawnParameters SpawnParams;
						World->SpawnActor<ABlock>(SolidBlockBPClass, FVector(BlockPosition.X, BlockPosition.Y, GetActorLocation().Z), FRotator::ZeroRotator, SpawnParams);
					}
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

void ALevelGrid::EnterCell(ICellOccupantInterface* CellOccuppant, FVector2D Cell)
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

void ALevelGrid::ExitCell(ICellOccupantInterface* CellOccuppant, FVector2D Cell)
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

void ALevelGrid::ChangeCell(ICellOccupantInterface* CellOccuppant, FVector2D OldCell, FVector2D NewCell)
{
	ExitCell(CellOccuppant, OldCell);
	EnterCell(CellOccuppant, NewCell);
}

FVector2D ALevelGrid::GetCellFromWorldCoordinates(FVector2D WorldCoordinates)
{
	FVector2D Cell = FVector2D::ZeroVector;

	if (CellSize > 0)
	{
		FVector GridPosition = GetActorLocation();
		FVector2D GridPosition2D = FVector2D(GridPosition.X, GridPosition.Y);

		Cell = (WorldCoordinates - GridPosition2D) / CellSize;
	}

	return Cell;
}

FVector2D ALevelGrid::GetWorldCoordinatesFromCell(FVector2D Cell)
{
	FVector2D WorldCoordinates = FVector2D::ZeroVector;

	FVector GridPosition = GetActorLocation();
	FVector2D GridPosition2D = FVector2D(GridPosition.X, GridPosition.Y);

	WorldCoordinates = Cell*CellSize + GridPosition2D;

	return WorldCoordinates;
}

