// Fill out your copyright notice in the Description page of Project Settings.

#include "LevelGrid.h"
#include "CellOccupantInterface.h"


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

	//TODO

	return Cell;
}