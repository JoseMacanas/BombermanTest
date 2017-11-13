// Fill out your copyright notice in the Description page of Project Settings.

#include "BomberPawnAIController.h"
#include "Player/BomberPawn.h"
#include "Engine.h"
#include "LevelGrid.h"
#include "CellOccupantInterface.h"
#include "Bomb.h"
#include "Explosion.h"

// Sets default values
ABomberPawnAIController::ABomberPawnAIController()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}


// Called when the game starts or when spawned
void ABomberPawnAIController::BeginPlay()
{
	Super::BeginPlay();
}


// Called when the AI Controller takes ownership of the Pawn
void ABomberPawnAIController::Possess(APawn* InPawn)
{
	Super::Possess(InPawn);

	// Get references to the BomberPawn and CurrentLevelGrid for ease of access
	BomberPawn = Cast<ABomberPawn>(InPawn);

	if (BomberPawn)
	{
		CurrentLevelGrid = BomberPawn->CurrentLevelGrid;
	}

	RandomNumberGenerator.GenerateNewSeed();
}


// Provides a random walking direction in the form of a 2D int point with values of -1, 0 or 1 for each coordinate
// bAvoidCollisions discards walking directions that lead into an obstacle and then selects a random direction from the remaining posibilities
FIntPoint ABomberPawnAIController::GetRandomWalkingDirection(bool bAvoidCollisions)
{	
	FIntPoint NewWalkingDirection = FIntPoint((RandomNumberGenerator.RandRange(0, 1)*-2) + 1, (RandomNumberGenerator.RandRange(0, 1)*-2) + 1);
	if (bAvoidCollisions && CurrentLevelGrid && BomberPawn)
	{
		TArray<FIntPoint> PosibleDirections = CurrentLevelGrid->MovementDirections;

		for (int DirectionIndex = PosibleDirections.Num() -1 ; DirectionIndex >= 0; --DirectionIndex)
		{
			FIntPoint PosibleDirection = PosibleDirections[DirectionIndex];

			FIntPoint NextCell = BomberPawn->CurrentCell + PosibleDirection;
			if (!CurrentLevelGrid->IsCellWalkable(NextCell))
			{
				PosibleDirections.RemoveAt(DirectionIndex);
			}
		}
		
		if (PosibleDirections.Num() > 0)
		{
			NewWalkingDirection = PosibleDirections[RandomNumberGenerator.RandRange(0, PosibleDirections.Num() - 1)];
		}
	}

	return NewWalkingDirection;
}


// Called every frame
void ABomberPawnAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (BomberPawn)
	{
		if (!BomberPawn->bIsAlive)
		{
			return;
		}
		
		BomberPawn->MoveXAxis(WalkingDirection.X);
		BomberPawn->MoveYAxis(WalkingDirection.Y);
	
		// If currently waiting, attempt to move again
		if (WalkingDirection == FIntPoint::ZeroValue)
		{
			WalkingDirection = GetRandomWalkingDirection(true);			
		}

		if (CurrentLevelGrid)
		{
			if (!AvoidDangers())
			{
				// If a bomb can be placed and the player can run away from it, place a bomb
				if (CurrentLevelGrid->CellHasEscapeRoute(BomberPawn->CurrentCell))
				{
					BomberPawn->PlaceBomb();
				}
				// Otherwise, wait
				else if (!CurrentLevelGrid->IsCellWalkable(BomberPawn->CurrentCell + WalkingDirection) 
					|| !CurrentLevelGrid->IsCellSafe(BomberPawn->CurrentCell + WalkingDirection) )
				{
					WalkingDirection = FIntPoint::ZeroValue;
				}
			}
		}
	}
}


// Check the Grid for any dangers (Bombs or Explosions) and set WalkingDirection to avoid them
bool ABomberPawnAIController::AvoidDangers()
{
	bool bRunningFromObstacle = false;

	if (CurrentLevelGrid)
	{
		TArray<ICellOccupantInterface*> Obstacles = CurrentLevelGrid->GetObstaclesFromCell(BomberPawn->CurrentCell);		

		for (int ObstacleIndex = 0; ObstacleIndex < Obstacles.Num(); ++ObstacleIndex)
		{
			ICellOccupantInterface* Obstacle = Obstacles[ObstacleIndex];

			ABomb* Bomb = Cast<ABomb>(Obstacle);
			AExplosion* Explosion = Cast<AExplosion>(Obstacle);

			FIntPoint DangerCell;
			if (Bomb)
			{
				DangerCell = Bomb->CurrentCell;
			}
			else if (Explosion)
			{
				DangerCell = Explosion->CurrentCell;
			}

			if (Bomb || Explosion)
			{
				bRunningFromObstacle = true;
				if (DangerCell == BomberPawn->CurrentCell
					&& !CurrentLevelGrid->IsCellWalkable(BomberPawn->CurrentCell + FIntPoint(WalkingDirection.X, 0))
					&& !CurrentLevelGrid->IsCellWalkable(BomberPawn->CurrentCell + FIntPoint(0, WalkingDirection.Y)))
				{
					WalkingDirection = GetRandomWalkingDirection(true);
				}
				else if (DangerCell != BomberPawn->CurrentCell)
				{
					WalkingDirection = BomberPawn->CurrentCell - DangerCell;

					WalkingDirection = FIntPoint(FMath::Sign(WalkingDirection.X), FMath::Sign(WalkingDirection.Y));

					FIntPoint PerpendicularDirection1 = FIntPoint(WalkingDirection.Y, WalkingDirection.X);
					FIntPoint PerpendicularDirection2 = FIntPoint(WalkingDirection.Y, WalkingDirection.X) *-1;

					if (CurrentLevelGrid->IsCellWalkable(BomberPawn->CurrentCell + PerpendicularDirection1))
					{
						WalkingDirection = PerpendicularDirection1;
					}
					else if (CurrentLevelGrid->IsCellWalkable(BomberPawn->CurrentCell + PerpendicularDirection2))
					{
						WalkingDirection = PerpendicularDirection2;
					}
				}
				break;
			}
		}
	}

	return bRunningFromObstacle;
}