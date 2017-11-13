// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "CellOccupantInterface.h"
#include "BomberPawn.generated.h"

class ABomb;
class ALevelGrid;

UCLASS()
class BOMBERMANTEST_API ABomberPawn : public APawn, public ICellOccupantInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ABomberPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Prepare BomberPawn properties for a new round (explosion range, number of bombs available, etc);
	void Reset();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// Apply current velocity, every tick, respecting Grid restrictions
	void Move(float DeltaTime);

	// Spawn a BomberPawn inside a Cell in the Grid
	void PlaceInGrid(ALevelGrid* LevelGrid, FIntPoint StartingCell, int PlayerIndex);
	
	// Input
	void MoveXAxis(float AxisValue);
	void MoveYAxis(float AxisValue);
	void PlaceBomb();
	
	// Called by Bombs when they explode or are removed from the game, to increase the number of usable Bombs from the Bomb pool
	void ReleaseBomb();
	
	// Player properties for the current round, to be called from Pickups
	void IncreaseAvailableBombs(int Increase);
	void IncreaseExplosionRange(int Increase);
	void IncreaseSpeed(float Increase);
	void IncreaseRemoteBombsTime(float Increase);
	
	// ICellOccupantInterface //
	virtual bool OnDamaged() override;
	virtual bool RemoveFromGame() override;
	virtual bool IsWalkable() const override;
	virtual void SetCurrentCell(FIntPoint Cell) override;
	virtual const FIntPoint GetCurrentCell() const override;
	// End ICellOccupantInterface //
	
	// Means that the player is in the game, in the grid and able to move and place Bombs
	UPROPERTY()
	bool bIsAlive = false;
	
	// Set by the input functions and read by the function Move every tick
	UPROPERTY()
	FVector CurrentVelocity;

	// The player Index in the current Grid
	UPROPERTY()
	int PlayerId = 0;

	// The player name displayed in the Scores screen
	UPROPERTY(EditInstanceOnly)
	FString PlayerName;

	// Player properties for the current round
	UPROPERTY()
	float CurrentRemoteBombsTimer;

	UPROPERTY()
	float CurrentSpeed;

	UPROPERTY()
	int CurrentSpeedIncreases;

	UPROPERTY()
	int CurrentAvailableBombs;

	UPROPERTY()
	int CurrentExplosionRange;
	//////////////////

	// Max Player properties
	UPROPERTY(EditDefaultsOnly)
	int MaxSpeedIncreases = 5;

	UPROPERTY(EditDefaultsOnly)
	int MaxBombs = 10;

	UPROPERTY(EditDefaultsOnly)
	int MaxExplosionRange = 10;
	//////////////////
	
	// Starting Player properties
	UPROPERTY(EditDefaultsOnly)
	int StartingBombs = 1;

	UPROPERTY(EditDefaultsOnly)
	int StartingExplosionRange = 2;

	UPROPERTY(EditDefaultsOnly)
	int StartingSpeed = 200;
	//////////////////

	// Bombs that have been placed
	UPROPERTY()
	int PlacedBombs;
	
	// A pool of Bombs is created when creating a character to avoid creating and destroying Bombs continuously;
	UPROPERTY()
	TArray<ABomb*> BombPool;

	// Bomb Blueprint class;
	UPROPERTY(EditDefaultsOnly)
	UClass* BombBPClass;
	
	UPROPERTY()
	ALevelGrid* CurrentLevelGrid;

	UPROPERTY()
	FIntPoint CurrentCell;
};
