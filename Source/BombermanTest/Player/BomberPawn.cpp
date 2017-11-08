// Fill out your copyright notice in the Description page of Project Settings.

#include "BomberPawn.h"
#include "Runtime/Engine/Classes/Components/InputComponent.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "Bomb.h"
#include "LevelGrid.h"
#include "Engine.h"


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
				BombPool[bombIndex] = World->SpawnActor<ABomb>(BombBPClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
			}
		}
	}

	if (LevelGrid)
	{
		FVector2D StartingLocation2D = LevelGrid->GetWorldCoordinatesFromCell(StartingCell);
		FVector StartingLocation = FVector(StartingLocation2D.X, StartingLocation2D.Y, LevelGrid->GetActorLocation().Z);
		SetActorLocation(StartingLocation);
		CurrentCell = StartingCell;
	}
}


// Called every frame
void ABomberPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!CurrentVelocity.IsZero())
	{
		FVector CurrentActorLocation = GetActorLocation();
		FVector2D CurrentActorLocation2D = FVector2D(CurrentActorLocation.X, CurrentActorLocation.Y);
		FVector DeltaMovement = CurrentVelocity * DeltaTime;

		if (LevelGrid)
		{
			FVector2D CurrentCellWorldLocation = LevelGrid->GetWorldCoordinatesFromCell(CurrentCell);
			FVector2D CellOffset = CurrentCellWorldLocation - CurrentActorLocation2D;

			FVector2D NextHorizontalLocation = CurrentActorLocation2D + FVector2D((FMath::Sign(DeltaMovement.X)*LevelGrid->CellSize*0.5f + DeltaMovement.X), 0);
			FVector2D NextVerticalLocation = CurrentActorLocation2D + FVector2D(0,(FMath::Sign(DeltaMovement.Y)*LevelGrid->CellSize*0.5f + DeltaMovement.Y));

			FIntPoint NextHorizontalCell = LevelGrid->GetCellFromWorldCoordinates(NextHorizontalLocation);
			FIntPoint NextVerticalCell = LevelGrid->GetCellFromWorldCoordinates(NextVerticalLocation);


			// Prevent moving into a Bomb or into a wall, but allow moving around on top of a Bomb that was just placed
			if (NextHorizontalCell != CurrentCell && !LevelGrid->IsCellWalkable(NextHorizontalCell))
			{
				DeltaMovement.X = 0;
			}
			if (NextVerticalCell != CurrentCell && !LevelGrid->IsCellWalkable(NextVerticalCell))
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
			FIntPoint NewCell = LevelGrid->GetCellFromWorldCoordinates(NewLocation2D);
			if (NewCell != CurrentCell)
			{
				LevelGrid->ChangeCell(this, CurrentCell, NewCell);
				CurrentCell = NewCell;

			}
		}
	}
}

// Called to bind functionality to input
void ABomberPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("PlaceBomb", IE_Pressed, this, &ABomberPawn::PlaceBomb);
	PlayerInputComponent->BindAxis("XAxis", this, &ABomberPawn::MoveXAxis);
	PlayerInputComponent->BindAxis("YAxis", this, &ABomberPawn::MoveYAxis);
}


void ABomberPawn::MoveXAxis(float AxisValue)
{
	CurrentVelocity.X = FMath::Clamp(AxisValue, -1.0f, 1.0f) * CurrentSpeed;
}

void ABomberPawn::MoveYAxis(float AxisValue)
{
	CurrentVelocity.Y = FMath::Clamp(AxisValue, -1.0f, 1.0f) * CurrentSpeed;
}


void ABomberPawn::PlaceBomb()
{	
	ABomb* AvailableBomb = NULL;
	for (int bombIndex = 0; bombIndex < BombPool.Num(); ++bombIndex)
	{
		if (BombPool[bombIndex])
		{
			if (!BombPool[bombIndex]->bPlacedInWorld)
			{
				AvailableBomb = BombPool[bombIndex];
				break;
			}
		}
	}

	if (AvailableBomb)
	{
		FVector ActorLocation = GetActorLocation();
		FVector2D ActorLocation2D = FVector2D(ActorLocation.X, ActorLocation.Y);
		if (LevelGrid)
		{
			FIntPoint CurrentCell = LevelGrid->GetCellFromWorldCoordinates(ActorLocation2D);
			if (AvailableBomb->PlaceInWorld(LevelGrid, CurrentCell))
			{
				if (GEngine)
				{
					GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, FString::Printf(TEXT("Bomb placed at %d - %d!"), CurrentCell.X, CurrentCell.Y));
				}
			}	
		}
	}
}


bool ABomberPawn::OnDamaged()
{
	// TODO: Die
	return true;
}
