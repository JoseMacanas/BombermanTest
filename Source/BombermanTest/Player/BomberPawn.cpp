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
}

// Called every frame
void ABomberPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!CurrentVelocity.IsZero())
	{
		FVector NewLocation = GetActorLocation() + (CurrentVelocity * DeltaTime);
		NewLocation.Z = 0;
		SetActorLocation(NewLocation);
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