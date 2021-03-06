// Fill out your copyright notice in the Description page of Project Settings.

#include "BomberPawn.h"
#include "Runtime/Engine/Classes/Components/InputComponent.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "Bomb.h"
#include "LevelGrid.h"
#include "Engine.h"
#include "BombermanTestGameStateBase.h"


// Sets default values
ABomberPawn::ABomberPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}


// Called when the game starts or when spawned
void ABomberPawn::BeginPlay()
{
	Super::BeginPlay();

	// Hide BomberPawn until it is placed in the grid
	SetActorHiddenInGame(true);
	SetActorTickEnabled(false);

	// Create Bomb pool
	if (BombBPClass)
	{
		UWorld* const World = GetWorld();
		if (World)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Instigator = this;

			BombPool.SetNum(MaxBombs, false);
			for (int bombIndex = 0; bombIndex < MaxBombs; ++bombIndex)
			{
				ABomb* Bomb = World->SpawnActor<ABomb>(BombBPClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
				if (Bomb)
				{
					BombPool[bombIndex] = Bomb;
					Bomb->OwnerPlayer = this;
				}
			}
		}
	}
}


// Prepare BomberPawn properties for a new round (explosion range, number of bombs available, etc);
void ABomberPawn::Reset()
{
	bIsAlive = true;
	SetActorHiddenInGame(false);
	SetActorTickEnabled(true);

	CurrentRemoteBombsTimer = 0.f;
	CurrentSpeedIncreases = 0;
	CurrentSpeed = StartingSpeed;
	CurrentAvailableBombs = StartingBombs;
	CurrentExplosionRange = StartingExplosionRange;
	PlacedBombs = 0;
}


// Called every frame
void ABomberPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CurrentRemoteBombsTimer > 0)
	{
		CurrentRemoteBombsTimer -= DeltaTime;
	}

	if (bIsAlive)
	{
		Move(DeltaTime);
	}	
}


// Called to bind functionality to input
// This is not really used in the prototype since the real bindings for the keyboard are handled in the InputManagerPawn class, 
// but it is left implemented because it allows gamepad support
void ABomberPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("PlaceBomb", IE_Pressed, this, &ABomberPawn::PlaceBomb);
	PlayerInputComponent->BindAxis("XAxis", this, &ABomberPawn::MoveXAxis);
	PlayerInputComponent->BindAxis("YAxis", this, &ABomberPawn::MoveYAxis);
}


// Apply current velocity, every tick, respecting Grid restrictions
void ABomberPawn::Move(float DeltaTime)
{
	if (!bIsAlive)
	{
		return;
	}

	if (!CurrentVelocity.IsZero())
	{
		FVector CurrentActorLocation = GetActorLocation();
		FVector2D CurrentActorLocation2D = FVector2D(CurrentActorLocation.X, CurrentActorLocation.Y);
		FVector DeltaMovement = CurrentVelocity * DeltaTime;

		if (CurrentLevelGrid)
		{
			FVector2D CurrentCellWorldLocation = CurrentLevelGrid->GetWorldCoordinatesFromCell(CurrentCell);
			FVector2D CellOffset = CurrentCellWorldLocation - CurrentActorLocation2D;

			FVector2D NextHorizontalLocation = CurrentActorLocation2D + FVector2D((FMath::Sign(DeltaMovement.X)*CurrentLevelGrid->CellSize*0.5f + DeltaMovement.X), 0);
			FVector2D NextVerticalLocation = CurrentActorLocation2D + FVector2D(0, (FMath::Sign(DeltaMovement.Y)*CurrentLevelGrid->CellSize*0.5f + DeltaMovement.Y));

			FIntPoint NextHorizontalCell = CurrentLevelGrid->GetCellFromWorldCoordinates(NextHorizontalLocation);
			FIntPoint NextVerticalCell = CurrentLevelGrid->GetCellFromWorldCoordinates(NextVerticalLocation);


			// Prevent moving into a Bomb or into a wall, but allow moving around on top of a Bomb that was just placed
			if (NextHorizontalCell != CurrentCell && !CurrentLevelGrid->IsCellWalkable(NextHorizontalCell))
			{
				DeltaMovement.X = 0;
			}
			if (NextVerticalCell != CurrentCell && !CurrentLevelGrid->IsCellWalkable(NextVerticalCell))
			{
				DeltaMovement.Y = 0;
			}

			// Arbitrarily give priority to one of the valid movement axes to prevent walking directions to compete when pressing two different directions (ex. up and down) at a junction
			if (DeltaMovement.X != 0)
			{
				DeltaMovement.Y = 0;
			}

			// Center the character in the grid when changing axis of movement, with a little bit of tolerance to compensate for floating point errors
			const float AxisChangeTolerance = 0.001f;
			if (DeltaMovement.X != 0 && FMath::Abs(CellOffset.Y) > AxisChangeTolerance)
			{
				DeltaMovement.Y = FMath::Min(FMath::Abs(DeltaMovement.X), FMath::Abs(CellOffset.Y)) * FMath::Sign(CellOffset.Y);
				DeltaMovement.X = 0;
			}
			else if (DeltaMovement.Y != 0 && FMath::Abs(CellOffset.X) > AxisChangeTolerance)
			{
				DeltaMovement.X = FMath::Min(FMath::Abs(DeltaMovement.Y), FMath::Abs(CellOffset.X)) * FMath::Sign(CellOffset.X);
				DeltaMovement.Y = 0;
			}

			FVector NewLocation = CurrentActorLocation + (DeltaMovement);
			SetActorLocation(NewLocation);

			FVector2D NewLocation2D = FVector2D(NewLocation.X, NewLocation.Y);
			FIntPoint NewCell = CurrentLevelGrid->GetCellFromWorldCoordinates(NewLocation2D);
			if (NewCell != CurrentCell)
			{
				CurrentLevelGrid->ChangeCell(this, CurrentCell, NewCell);
				SetCurrentCell(NewCell);
			}
		}
	}
}


// Spawn a BomberPawn inside a Cell in the Grid
void ABomberPawn::PlaceInGrid(ALevelGrid* LevelGrid, FIntPoint StartingCell, int PlayerIndex)
{
	if (LevelGrid)
	{
		CurrentLevelGrid = LevelGrid;

		PlayerId = PlayerIndex;

		FVector2D StartingLocation2D = CurrentLevelGrid->GetWorldCoordinatesFromCell(StartingCell);
		FVector StartingLocation = FVector(StartingLocation2D.X, StartingLocation2D.Y, CurrentLevelGrid->GetActorLocation().Z);
		SetActorLocation(StartingLocation);
		SetCurrentCell(StartingCell);
		CurrentLevelGrid->EnterCell(this, CurrentCell);

		Reset();
	}
}


// Input
void ABomberPawn::MoveXAxis(float AxisValue)
{
	CurrentVelocity.X = FMath::Clamp(AxisValue, -1.0f, 1.0f) * CurrentSpeed;
}

// Input
void ABomberPawn::MoveYAxis(float AxisValue)
{
	CurrentVelocity.Y = FMath::Clamp(AxisValue, -1.0f, 1.0f) * CurrentSpeed;
}


// Places a Bomb or makes an existing Bomb explode if Remote Bombs Pickup timer > 0
void ABomberPawn::PlaceBomb()
{
	if (!bIsAlive)
	{
		return;
	}
	ABomb* AvailableBomb = NULL;
	for (int bombIndex = 0; bombIndex < BombPool.Num(); ++bombIndex)
	{
		if (BombPool[bombIndex])
		{
			bool bIsPlacedInWorld = BombPool[bombIndex]->bPlacedInWorld;

			if (CurrentRemoteBombsTimer > 0 && bIsPlacedInWorld)
			{
				BombPool[bombIndex]->Explode();
				return;
			}
			else if (!bIsPlacedInWorld)
			{
				AvailableBomb = BombPool[bombIndex];

				if (CurrentRemoteBombsTimer <= 0)
				{
					break;
				}
			}
		}
	}

	if (CurrentAvailableBombs <= PlacedBombs)
	{
		return;
	}

	if (AvailableBomb)
	{
		AvailableBomb->ExplosionSize = CurrentExplosionRange;
		FVector ActorLocation = GetActorLocation();
		FVector2D ActorLocation2D = FVector2D(ActorLocation.X, ActorLocation.Y);
		if (CurrentLevelGrid)
		{
			if (AvailableBomb->PlaceInWorld(CurrentLevelGrid, CurrentCell))
			{
				PlacedBombs++;
				//if (GEngine)
				//{
				//	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, FString::Printf(TEXT("Bomb placed at %d - %d!"), CurrentCell.X, CurrentCell.Y));
				//}
			}
		}
	}
}


// Called by Bombs when they explode or are removed from the game, to increase the number of usable Bombs from the Bomb pool
void ABomberPawn::ReleaseBomb()
{
	PlacedBombs--;
}


// Player properties for the current round, to be called from Pickups
void ABomberPawn::IncreaseAvailableBombs(int Increase)
{
	if (CurrentAvailableBombs < MaxBombs)
	{
		CurrentAvailableBombs += Increase;
	}
}

void ABomberPawn::IncreaseExplosionRange(int Increase)
{
	if (CurrentExplosionRange < MaxExplosionRange)
	{
		CurrentExplosionRange += Increase;
	}	
}

void ABomberPawn::IncreaseSpeed(float Increase)
{
	if (CurrentSpeedIncreases < MaxSpeedIncreases)
	{
		CurrentSpeed += Increase;
		CurrentSpeedIncreases++;
	}
}

void ABomberPawn::IncreaseRemoteBombsTime(float Increase)
{
	CurrentRemoteBombsTimer += Increase;
}



// ICellOccupantInterface //
bool ABomberPawn::OnDamaged()
{
	if (bIsAlive)
	{
		RemoveFromGame();
		return true;
	}

	return false;
}


bool ABomberPawn::RemoveFromGame()
{
	if (CurrentLevelGrid)
	{
		CurrentLevelGrid->ExitCell(this, CurrentCell);
	}

	bIsAlive = false;
	SetActorHiddenInGame(true);
	SetActorTickEnabled(false);

	return true;
}

bool ABomberPawn::IsWalkable() const
{
	return true;
}

void ABomberPawn::SetCurrentCell(FIntPoint Cell)
{
	CurrentCell = Cell;
}

const FIntPoint ABomberPawn::GetCurrentCell() const
{
	return CurrentCell;
}