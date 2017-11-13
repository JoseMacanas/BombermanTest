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


// Clear the current grid, generate a new one and place the players on it 
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


// Locate the Grid corners, depending on the Grid size, to be used for the initial positions of the Players
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


// Set all the valid movement directions in the grid (North, South, East, West)
void ALevelGrid::SetMovementDirections()
{
	MovementDirections.Empty();
	// Movement in the grid is only allowed in these four directions:
	
	MovementDirections.Add(FIntPoint(0, 1)); // North
	MovementDirections.Add(FIntPoint(0, -1)); // South
	MovementDirections.Add(FIntPoint(1, 0)); // West
	MovementDirections.Add(FIntPoint(-1, 0)); // East
}


// Place players in the grid corners
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


// Spawns a random pickup from the available pickup clases in the specified Cell
void ALevelGrid::SpawnRandomPickup(FIntPoint Cell)
{
	bool bShouldSpawnPickup = RandomNumberGenerator.RandRange(0, 9) <= 2;

	if (bShouldSpawnPickup)
	{
		if (PickupBPClasses.Num() > 0)
		{
			// Change the random function to make certain pickups more likely to spawn
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

// Spawn a single block in the Grid
void ALevelGrid::SpawnBlock(UWorld* const World, FIntPoint Cell, UClass* BlockBPClass, FActorSpawnParameters SpawnParams)
{
	if (World)
	{
		FVector2D BlockPosition2D = GetWorldCoordinatesFromCell(Cell);
		FVector BlockPosition = FVector(BlockPosition2D.X, BlockPosition2D.Y, GetActorLocation().Z);

		ABlock* Block = World->SpawnActor<ABlock>(BlockBPClass, BlockPosition, FRotator::ZeroRotator, SpawnParams);
		Block->CurrentLevelGrid = this;
		Block->SetCurrentCell(Cell);

		EnterCell(Block, Cell);
	}
}

// Procedurally generate a new grid
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
	
	FActorSpawnParameters SpawnParams;

	UWorld* const World = GetWorld();
	if (World && BorderBlockBPClass && SolidBlockBPClass && BreakableBlockBPClass)
	{
		for (int RowIndex = 0; RowIndex < GridHeight + 2; ++RowIndex)
		{
			for (int ColumnIndex = 0; ColumnIndex < GridWidth + 2; ++ColumnIndex)
			{
				FIntPoint Cell = FIntPoint(RowIndex, ColumnIndex);
				
				if (ColumnIndex == 0 || RowIndex == 0 || ColumnIndex == GridWidth + 1 || RowIndex == GridHeight + 1)
				{
					SpawnBlock(World, Cell, BorderBlockBPClass, SpawnParams);
				}
				else if (RowIndex % 2 == 0 && ColumnIndex % 2 == 0)
				{
					SpawnBlock(World, Cell, SolidBlockBPClass, SpawnParams);
				}
				else if (!ClearPositions.Contains(Cell))
				{
					if (RandomNumberGenerator.RandRange(0, 10) > 0) // Change this to change the chances of breakable blocks
					{
						SpawnBlock(World, Cell, BreakableBlockBPClass, SpawnParams);
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


// A new CellOccuppant occupies a Cell, causing reactions on the existing CellOccupants
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


// Remove a CellOccuppant from a Cell
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


// Move a CellOccuppant from one cell to another
void ALevelGrid::ChangeCell(ICellOccupantInterface* CellOccuppant, FIntPoint OldCell, FIntPoint NewCell)
{
	ExitCell(CellOccuppant, OldCell);
	EnterCell(CellOccuppant, NewCell);
}


// Grid coordinate conversion
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

// Grid coordinate conversion
FVector2D ALevelGrid::GetWorldCoordinatesFromCell(FIntPoint Cell) const
{
	FVector2D WorldCoordinates = FVector2D::ZeroVector;

	FVector GridPosition = GetActorLocation();
	FVector2D GridPosition2D = FVector2D(GridPosition.X, GridPosition.Y);

	WorldCoordinates = FVector2D(Cell)*CellSize + GridPosition2D;

	return WorldCoordinates;
}


// Returns true if the Cell doesn't contain any CellOccuppant that blocks player movement
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


// Spawns an explosion blast and all subsequent explosion blasts affected by it
// Also keeps track of players killed in the reaction and updates the GameState accordingly
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
			SpawnExplosionBlast(CurrentBomb, AffectedBombs, AffectedPlayers);
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


// Spawns a single explosion blast
void ALevelGrid::SpawnExplosionBlast(ABomb* Bomb, TArray<ABomb*>& InOutAffectedBombs, TArray<int>& InOutAffectedPlayers)
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

		bool bExplosionGoesThrough = SpawnExplosion(GetWorld(), SpawnParams, Cell, InOutAffectedBombs, InOutAffectedPlayers);

		for (int ExplosionRadiusIndex = 1; ExplosionRadiusIndex < ExplosionSize; ++ExplosionRadiusIndex)
		{
			for (int DirectionIndex = ExplosionDirections.Num() -1; DirectionIndex >= 0; --DirectionIndex)
			{
				FIntPoint NextAffectedCell = Cell + (ExplosionDirections[DirectionIndex] * ExplosionRadiusIndex);
				
				bool bExplosionGoesThrough = SpawnExplosion(GetWorld(), SpawnParams, NextAffectedCell, InOutAffectedBombs, InOutAffectedPlayers);

				if (!bExplosionGoesThrough)
				{
					ExplosionDirections.RemoveAt(DirectionIndex);
				}
			}
		}
	}
}


// Returns true if the fire goes through (it isn't blocked), and false if the explosion fire is blocked at this location
bool ALevelGrid::SpawnExplosion(UWorld* const World, FActorSpawnParameters SpawnParams, FIntPoint Cell, TArray<ABomb*>& InOutAffectedBombs, TArray<int>& InOutAffectedPlayers)
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
				InOutAffectedBombs.Add(CellBomb);
			}
			else if (CellPlayer)
			{
				CellPlayer->OnDamaged();
				InOutAffectedPlayers.Add(CellPlayer->PlayerId);
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


// Get the names of the players placed in the grid
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


// Get the obstacles in the current Cell
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


// Get the obstacles in all four possible movement directions from the current Cell
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


// True if the Cell is not in sight of a bomb
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


// True if there is a reachable Cell that would protect from a bomb placed in the specified Cell
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


