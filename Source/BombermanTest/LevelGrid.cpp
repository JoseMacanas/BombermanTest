// Fill out your copyright notice in the Description page of Project Settings.

#include "LevelGrid.h"
#include "CellOccupantInterface.h"
#include "Block.h"
#include "Bomb.h"
#include "Explosion.h"
#include "Player/BomberPawn.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "Engine.h"
#include "Pickups/Pickup.h"
#include "BombermanTestGameStateBase.h"


// Sets default values
ALevelGrid::ALevelGrid()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SetMovementDirections();
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
	
	SetStartingPositions();
	GenerateLevel();
	PlacePlayers();
}

void ALevelGrid::SetStartingPositions()
{
	StartingPositions.Empty();

	// The grid real valid positions go from (1,1) to (GridWidth, GridHeight), to take into account the borders of the grid
	// Place up to four players in the corners.
	StartingPositions.Add(FIntPoint(1, 1));
	StartingPositions.Add(FIntPoint(GridWidth, GridHeight));
	StartingPositions.Add(FIntPoint(1, GridHeight));
	StartingPositions.Add(FIntPoint(GridWidth, 1));
}

void ALevelGrid::SetMovementDirections()
{
	MovementDirections.Empty();
	// Movement in the grid is only allowed in these four directions:
	
	MovementDirections.Add(FIntPoint(0, 1)); // North
	MovementDirections.Add(FIntPoint(0, -1)); // South
	MovementDirections.Add(FIntPoint(1, 0)); // West
	MovementDirections.Add(FIntPoint(-1, 0)); // East
}



void ALevelGrid::PlacePlayers()
{
	for (int PlayerIndex = 0; PlayerIndex < Players.Num(); ++PlayerIndex)
	{
		ABomberPawn* Player = Players[PlayerIndex];
		if (Player)
		{
			Player->PlaceInGrid(this, StartingPositions[PlayerIndex], PlayerIndex);
		}
	}
}


void ALevelGrid::SpawnRandomPickup(FIntPoint Cell)
{
	bool bShouldSpawnPickup = RandomNumberGenerator.RandRange(0, 9) <= 2;

	if (bShouldSpawnPickup)
	{
		if (PickupBPClasses.Num() > 0)
		{
			// Change the random function to make certainpickups more likely to spawn
			int PickupIndex = RandomNumberGenerator.RandRange(0, PickupBPClasses.Num() - 1);

			UClass * PickupBPClass = PickupBPClasses[PickupIndex];
			if (PickupBPClasses[PickupIndex])
			{
				UWorld* const World = GetWorld();
				if (World)
				{
					FActorSpawnParameters SpawnParams;

					FVector2D CellWorldLocation = GetWorldCoordinatesFromCell(Cell);
					APickup* NewPickup = World->SpawnActor<APickup>(PickupBPClass, FVector(CellWorldLocation.X, CellWorldLocation.Y, GetActorLocation().Z), FRotator::ZeroRotator, SpawnParams);
					NewPickup->CurrentLevelGrid = this;
					NewPickup->SetCurrentCell(Cell);

					EnterCell(NewPickup, Cell);
				}
			}
		}
	}
}

void ALevelGrid::GenerateLevel(int RandomSeed)
{	
	//RandomNumberGenerator.Initialize(RandomSeed); // DEBUG

	RandomNumberGenerator.GenerateNewSeed();
	
	TArray<FIntPoint> ClearPositions = StartingPositions;
	for (int StartingPositionIndex = 0; StartingPositionIndex < StartingPositions.Num(); ++StartingPositionIndex)
	{
		for (int MovementDirectionIndex = 0; MovementDirectionIndex < MovementDirections.Num(); ++MovementDirectionIndex)
		{
			ClearPositions.Add(StartingPositions[StartingPositionIndex] + MovementDirections[MovementDirectionIndex]);
		}
	}
	
	UWorld* const World = GetWorld();
	if (World && BorderBlockBPClass && SolidBlockBPClass && BreakableBlockBPClass)
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
					Block->CurrentLevelGrid = this;
					Block->CurrentCell = Cell;

					EnterCell(Block, Cell);
				}
				else if (RowIndex % 2 == 0 && ColumnIndex % 2 == 0)
				{
					FActorSpawnParameters SpawnParams;
					ABlock* Block = World->SpawnActor<ABlock>(SolidBlockBPClass, FVector(BlockPosition.X, BlockPosition.Y, GetActorLocation().Z), FRotator::ZeroRotator, SpawnParams);
					Block->CurrentLevelGrid = this;
					Block->CurrentCell = Cell;

					EnterCell(Block, Cell);
				}
				else if (!ClearPositions.Contains(Cell))
				{
					if (RandomNumberGenerator.RandRange(0, 10) > 0) // Change this to change the chances of breakable blocks
					{
						FActorSpawnParameters SpawnParams;
						ABlock* Block = World->SpawnActor<ABlock>(BreakableBlockBPClass, FVector(BlockPosition.X, BlockPosition.Y, GetActorLocation().Z), FRotator::ZeroRotator, SpawnParams);
						Block->CurrentLevelGrid = this;
						Block->CurrentCell = Cell;

						EnterCell(Block, Cell);
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

	bool bIsCellOnFire = false;

	ABomberPawn* Player = Cast<ABomberPawn>(CellOccuppant);
	TArray<APickup*> Pickups;

	for (auto CellSetIterator = CellSet.CreateIterator(); CellSetIterator; ++CellSetIterator)
	{
		if (Cast<AExplosion>(*CellSetIterator))
		{
			bIsCellOnFire = true;
			break;
		}
		else if (Cast<APickup>(*CellSetIterator))
		{
			Pickups.Add(Cast<APickup>(*CellSetIterator));
		}
	}

	if (Player)
	{
		for (int PickupIterator = 0; PickupIterator < Pickups.Num(); ++PickupIterator)
		{
			APickup* Pickup = Pickups[PickupIterator];
			Pickup->ApplyEffects(Player);
			Pickup->OnDamaged();
		}
	}

	if (bIsCellOnFire)
	{
		for (auto& Elem : CellSet)
		{
			Elem->OnDamaged();
		}

		// Notify gamestate of players dying when they walk on an existing explosion
		if (Player)
		{
			UWorld* const World = GetWorld();
			if (World)
			{
				ABombermanTestGameStateBase* GameState = World->GetGameState<ABombermanTestGameStateBase>();

				if (GameState)
				{
					TArray<int> AffectedPlayers;
					AffectedPlayers.Add(Player->PlayerId);
					GameState->OnPlayerDeath(AffectedPlayers);
				}
			}
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
		
		for (auto CellSetIterator = CellSet.CreateConstIterator(); CellSetIterator; ++CellSetIterator)
		{
			const ICellOccupantInterface* CellOccupant = *CellSetIterator;
			if (CellOccupant)
			{
				if (!CellOccupant->IsWalkable())
				{
					return false;
				}
			}
		}
	}

	return true;
}


void ALevelGrid::SpawnChainReaction(ABomb* Bomb)
{
	TArray<ABomb*> AffectedBombs;
	TArray<int> AffectedPlayers;

	AffectedBombs.Add(Bomb);
	while (AffectedBombs.Num() > 0)
	{
		ABomb* CurrentBomb = AffectedBombs[AffectedBombs.Num() - 1];
		AffectedBombs.Remove(CurrentBomb);
		if (CurrentBomb)
		{
			SpawnExplosion(CurrentBomb, AffectedBombs, AffectedPlayers);
		}
	}

	if (AffectedPlayers.Num() > 0)
	{
		UWorld* const World = GetWorld();
		if (World)
		{
			ABombermanTestGameStateBase* GameState = World->GetGameState<ABombermanTestGameStateBase>();

			if (GameState)
			{
				GameState->OnPlayerDeath(AffectedPlayers);
			}
		}
	}
}

void ALevelGrid::SpawnExplosion(ABomb* Bomb, TArray<ABomb*>& AffectedBombs, TArray<int>& AffectedPlayers)
{
	if (!Bomb)
	{
		return;
	}
	FIntPoint Cell = Bomb->CurrentCell;
	int ExplosionSize = Bomb->ExplosionSize;
	Bomb->RemoveFromGame();

	if (ExplosionBPClass)
	{
		TArray<FIntPoint> ExplosionDirections = MovementDirections;
		FActorSpawnParameters SpawnParams;

		bool bExplosionGoesThrough = SpawnExplosionFire(GetWorld(), SpawnParams, Cell, AffectedBombs, AffectedPlayers);

		for (int ExplosionRadiusIndex = 1; ExplosionRadiusIndex < ExplosionSize; ++ExplosionRadiusIndex)
		{
			for (int DirectionIndex = ExplosionDirections.Num() -1; DirectionIndex >= 0; --DirectionIndex)
			{
				FIntPoint NextAffectedCell = Cell + (ExplosionDirections[DirectionIndex] * ExplosionRadiusIndex);
				
				bool bExplosionGoesThrough = SpawnExplosionFire(GetWorld(), SpawnParams, NextAffectedCell, AffectedBombs, AffectedPlayers);

				if (!bExplosionGoesThrough)
				{
					ExplosionDirections.RemoveAt(DirectionIndex);
				}
			}
		}
	}
}

bool ALevelGrid::SpawnExplosionFire(UWorld* const World, FActorSpawnParameters SpawnParams, FIntPoint Cell, TArray<ABomb*>& AffectedBombs, TArray<int>& AffectedPlayers)
{
	bool bExplosionGoesThrough = true;
	if (CellOccupants.Contains(Cell))
	{
		TSet<ICellOccupantInterface*> CellSet = CellOccupants[Cell];
		
		for (auto& Elem : CellSet)
		{
			ABomb* CellBomb = Cast<ABomb>(Elem);
			ABomberPawn* CellPlayer = Cast<ABomberPawn>(Elem);
			ABlock* CellBlock = Cast<ABlock>(Elem);
			if (CellBomb)
			{
				AffectedBombs.Add(CellBomb);
			}
			else if (CellPlayer)
			{
				CellPlayer->OnDamaged();
				AffectedPlayers.Add(CellPlayer->PlayerId);
			}
			else if (CellBlock)
			{
				CellBlock->OnDamaged();
				bExplosionGoesThrough = false;
			}
		}
	}

	if (bExplosionGoesThrough)
	{
		if (ExplosionBPClass && World)
		{
			FVector2D CellWorldLocation = GetWorldCoordinatesFromCell(Cell);
			AExplosion* Explosion = World->SpawnActor<AExplosion>(ExplosionBPClass, FVector(CellWorldLocation.X, CellWorldLocation.Y, GetActorLocation().Z), FRotator::ZeroRotator, SpawnParams);
			Explosion->CurrentLevelGrid = this;
			Explosion->CurrentCell = Cell;
		}
	}

	return bExplosionGoesThrough;
}

TArray<FString> ALevelGrid::GetPlayerNames()
{
	TArray<FString> PlayerNames;

	for (int PlayerIndex = 0; PlayerIndex < Players.Num(); ++PlayerIndex)
	{
		ABomberPawn* Player = Players[PlayerIndex];
		if (Player)
		{
			PlayerNames.Add(Player->PlayerName);
		}
	}

	return PlayerNames;
}


TArray<ICellOccupantInterface*> ALevelGrid::GetCellObstacles(FIntPoint Cell) const
{
	TArray<ICellOccupantInterface*> ObstaclesInCell;

	if (CellOccupants.Contains(Cell))
	{
		TSet<ICellOccupantInterface*> CellSet = CellOccupants[Cell];

		for (auto& Elem : CellSet)
		{
			ABomb* CellBomb = Cast<ABomb>(Elem);
			AExplosion* CellExplosion = Cast<AExplosion>(Elem);
			ABlock* CellBlock = Cast<ABlock>(Elem);
			if (CellBomb)
			{
				ObstaclesInCell.Add(CellBomb);
			}
			else if (CellBlock)
			{
				ObstaclesInCell.Add(CellBlock);
			}
			else if (CellExplosion)
			{
				ObstaclesInCell.Add(CellExplosion);
			}
		}
	}

	return ObstaclesInCell;
}

TArray<ICellOccupantInterface*> ALevelGrid::GetObstaclesFromCell(FIntPoint Cell) const
{
	TArray<ICellOccupantInterface*> ObstaclesFound;

	TArray<FIntPoint> PosibleDirections = MovementDirections;

	TArray<ICellOccupantInterface*> ObstaclesInCell = GetCellObstacles(Cell);
	ObstaclesFound.Append(ObstaclesInCell);

	if (ObstaclesInCell.Num() > 0)
	{
		return ObstaclesInCell;
	}

	for (int RadiusIndex = 1; PosibleDirections.Num() > 0; ++RadiusIndex)
	{
		for (int DirectionIndex = PosibleDirections.Num() - 1; DirectionIndex >= 0; --DirectionIndex)
		{
			FIntPoint NextCell = Cell + (PosibleDirections[DirectionIndex] * RadiusIndex);
			
			TArray<ICellOccupantInterface*> ObstaclesInCell = GetCellObstacles(NextCell);

			if (ObstaclesInCell.Num() > 0)
			{
				ObstaclesFound.Append(ObstaclesInCell);
				PosibleDirections.RemoveAt(DirectionIndex);
			}
		}
	}

	return ObstaclesFound;
}

bool ALevelGrid::IsCellSafe(FIntPoint Cell) const
{
	TArray<ICellOccupantInterface*> Obstacles = GetObstaclesFromCell(Cell);

	for (int ObstacleIndex = 0; ObstacleIndex < Obstacles.Num(); ++ObstacleIndex)
	{
		ICellOccupantInterface* Obstacle = Obstacles[ObstacleIndex];
		if (Obstacle)
		{
			ABomb* CellBomb = Cast<ABomb>(Obstacle);
			if (CellBomb)
			{
				return false;
			}
		}		
	}
	return true;
}

bool ALevelGrid::CellHasEscapeRoute(FIntPoint Cell) const
{
	TArray<FIntPoint> PosibleDirections = MovementDirections;

	for (int RadiusIndex = 1; PosibleDirections.Num() > 0; ++RadiusIndex)
	{
		for (int DirectionIndex = PosibleDirections.Num() - 1; DirectionIndex >= 0; --DirectionIndex)
		{
			FIntPoint NextCell = Cell + (PosibleDirections[DirectionIndex] * RadiusIndex);

			TArray<ICellOccupantInterface*> ObstaclesInCell = GetCellObstacles(NextCell);
			if (ObstaclesInCell.Num() > 0)
			{
				PosibleDirections.RemoveAt(DirectionIndex);
			}
			else
			{
				TArray<FIntPoint> AdjacentCellsDirections = MovementDirections;

				// Check only Cells perpendicular to the direction currently being checked
				AdjacentCellsDirections.Remove(PosibleDirections[DirectionIndex]);
				AdjacentCellsDirections.Remove(PosibleDirections[DirectionIndex] * -1);
				for (int AdjacentCellIndex = AdjacentCellsDirections.Num() -1; AdjacentCellIndex >= 0; --AdjacentCellIndex)
				{
					FIntPoint AdjacentCell = NextCell + AdjacentCellsDirections[AdjacentCellIndex];
					if (IsCellWalkable(AdjacentCell) && IsCellSafe(AdjacentCell))
					{
						return true;
					}
				}
			}
		}
	}

	return false;
}


