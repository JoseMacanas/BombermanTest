// Fill out your copyright notice in the Description page of Project Settings.

#include "Block.h"
#include "LevelGrid.h"
#include "Engine.h"

// Sets default values
ABlock::ABlock()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}


// Called when the game starts or when spawned
void ABlock::BeginPlay()
{
	Super::BeginPlay();	
}


// Called every frame
void ABlock::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


bool ABlock::OnDamaged()
{
	if (!bIsBreakable)
	{
		return false;
	}
	
	if (CurrentLevelGrid)
	{
		CurrentLevelGrid->SpawnRandomPickup(CurrentCell);
	}
	RemoveFromGame();
	return true;
}


bool ABlock::RemoveFromGame()
{
	if (CurrentLevelGrid)
	{		
		CurrentLevelGrid->ExitCell(this, CurrentCell);
	}

	Destroy();
	return true;
}


bool ABlock::IsWalkable() const
{
	return false;
}

void ABlock::SetCurrentCell(FIntPoint Cell)
{
	CurrentCell = Cell;
}

const FIntPoint ABlock::GetCurrentCell() const
{
	return CurrentCell;
}