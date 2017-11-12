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

void ABomberPawnAIController::Possess(APawn* InPawn)
{
	Super::Possess(InPawn);

	BomberPawn = Cast<ABomberPawn>(InPawn);

	if (BomberPawn)
	{
		CurrentLevelGrid = BomberPawn->CurrentLevelGrid;
	}

	RandomNumberGenerator.GenerateNewSeed();

	//if (GEngine)
	//{
	//	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, FString::Printf(TEXT("AI PAWN placed at %d - %d!"), WalkingDirection.X, WalkingDirection.Y));
	//}
}

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
	
		if (WalkingDirection == FIntPoint::ZeroValue)
		{
			WalkingDirection = GetRandomWalkingDirection(true);			
		}

		if (CurrentLevelGrid)
		{
			TArray<ICellOccupantInterface*> Obstacles = CurrentLevelGrid->GetObstaclesFromCell(BomberPawn->CurrentCell);

			bool bRunningFromObstacle = false;

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
				else if(Explosion)
				{
					DangerCell = Explosion->CurrentCell;
				}

				if (Bomb || Explosion)
				{
					bRunningFromObstacle = true;
					if ( DangerCell == BomberPawn->CurrentCell 
						&& !CurrentLevelGrid->IsCellWalkable(BomberPawn->CurrentCell + FIntPoint(WalkingDirection.X, 0))
							&& !CurrentLevelGrid->IsCellWalkable(BomberPawn->CurrentCell + FIntPoint(0, WalkingDirection.Y)) )
					{
						WalkingDirection = GetRandomWalkingDirection(true);
					}
					else if (DangerCell != BomberPawn->CurrentCell)
					{
						WalkingDirection = BomberPawn->CurrentCell - DangerCell;

						WalkingDirection = FIntPoint(FMath::Sign(WalkingDirection.X), FMath::Sign(WalkingDirection.Y));

						if (CurrentLevelGrid->IsCellWalkable(BomberPawn->CurrentCell + FIntPoint(WalkingDirection.Y, WalkingDirection.X)))
						{
							WalkingDirection = FIntPoint(WalkingDirection.Y, WalkingDirection.X);
						}
						else if (CurrentLevelGrid->IsCellWalkable(BomberPawn->CurrentCell + FIntPoint(WalkingDirection.Y, WalkingDirection.X) *-1))
						{
							WalkingDirection = FIntPoint(WalkingDirection.Y, WalkingDirection.X)*-1;
						}

						WalkingDirection = FIntPoint(FMath::Sign(WalkingDirection.X), FMath::Sign(WalkingDirection.Y));
	
					}
					break;
				}
			}
			if (!bRunningFromObstacle)
			{
				if (CurrentLevelGrid->CellHasEscapeRoute(BomberPawn->CurrentCell))
				{
					BomberPawn->PlaceBomb();
				}
				else if (!CurrentLevelGrid->IsCellWalkable(BomberPawn->CurrentCell + WalkingDirection) 
					|| !CurrentLevelGrid->IsCellSafe(BomberPawn->CurrentCell + WalkingDirection) )
				{
					WalkingDirection = FIntPoint::ZeroValue;
				}
			}
		}
	}
}