// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LevelGrid.generated.h"

class ICellOccupantInterface;
class ABomb;
class ABomberPawn;


UCLASS()
class BOMBERMANTEST_API ALevelGrid : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALevelGrid();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// A new CellOccuppant occupies a Cell, causing reactions on the existing CellOccupants
	void EnterCell(ICellOccupantInterface* CellOccuppant, FIntPoint Cell);

	// Remove a CellOccuppant from a Cell
	void ExitCell(ICellOccupantInterface* CellOccuppant, FIntPoint Cell);
	
	// Move a CellOccuppant from one cell to another
	void ChangeCell(ICellOccupantInterface* CellOccuppant, FIntPoint OldCell, FIntPoint NewCell);

	// Spawn a single block in the Grid
	void SpawnBlock(UWorld* const World, FIntPoint Cell, UClass* BlockBPClass, FActorSpawnParameters SpawnParams);

	// Procedurally generate a new grid
	void GenerateLevel(int RandomSeed = 0);

	// Place players in the grid corners
	void PlacePlayers();
	
	// Clear the current grid, generate a new one and place the players on it 
	void RestartGame();

	// Get the names of the players placed in the grid
	TArray<FString> GetPlayerNames();

	// Grid coordinate conversion
	FIntPoint GetCellFromWorldCoordinates(FVector2D WorldCoordinates) const;
	FVector2D GetWorldCoordinatesFromCell(FIntPoint Cell) const;

	// Returns true if the Cell doesn't contain any CellOccuppant that blocks player movement
	bool IsCellWalkable(FIntPoint Cell) const;

	// Spawns a random pickup from the available pickup clases in the specified Cell
	void SpawnRandomPickup(FIntPoint Cell);

	// Spawns an explosion blast and all subsequent explosion blasts affected by it
	// Also keeps track of players killed in the reaction and updates the GameState accordingly
	void SpawnChainReaction(ABomb* Bomb);
	
	// Provide the camera the current area of interest
	void GetCurrentAreaOfInterest(FVector& OutCenterOfInterest, FVector2D& OutSizeOfInterest);


	// AI Awareness functions //

	// True if there is a reachable Cell that would protect from a bomb placed in the specified Cell
	bool CellHasEscapeRoute(FIntPoint Cell) const;

	// True if the Cell is not in sight of a bomb
	bool IsCellSafe(FIntPoint Cell) const;

	// Get the obstacles in all four possible movement directions from the current Cell
	TArray<ICellOccupantInterface*> GetObstaclesFromCell(FIntPoint Cell) const;

protected:
	// Get the obstacles in the current Cell
	TArray<ICellOccupantInterface*> GetCellObstacles(FIntPoint Cell) const;

	// end AI Awareness functions //

protected:
	// Spawns a single explosion blast
	void SpawnExplosionBlast(ABomb* Bomb, TArray<ABomb*>& InOutAffectedBombs, TArray<int>& InOutAffectedPlayers);

	// Returns true if the fire goes through (it isn't blocked), and false if the explosion fire is blocked at this location
	bool SpawnExplosion(UWorld* const World, FActorSpawnParameters SpawnParams, FIntPoint Cell, TArray<ABomb*>& InOutAffectedBombs, TArray<int>& InOutAffectedPlayers);
	
	// Locate the Grid corners, depending on the Grid size, to be used for the initial positions of the Players
	void SetStartingPositions();

	// Set all the valid movement directions in the grid (North, South, East, West)
	void SetMovementDirections();

public:

	// Grid properties, to be edited in the Blueprint
	UPROPERTY(EditInstanceOnly)
	float CellSize = 100;

	UPROPERTY(EditInstanceOnly)
	int GridWidth = 5;

	UPROPERTY(EditInstanceOnly)
	int GridHeight = 5;
	
	// Any object placed in the Grid needs to implement the ICellOccupantInterface and will have a reference in this map
	// Can't be UPROPERTY because UINTERFACEs are not currently supported as element types.
	TMap<FIntPoint, TSet<ICellOccupantInterface*>> CellOccupants;

	// Blueprint classes for spawns
	UPROPERTY(EditDefaultsOnly)
	UClass* SolidBlockBPClass;

	UPROPERTY(EditDefaultsOnly)
	UClass* BorderBlockBPClass;

	UPROPERTY(EditDefaultsOnly)
	UClass* BreakableBlockBPClass;

	UPROPERTY(EditDefaultsOnly)
	UClass* ExplosionBPClass;

	UPROPERTY(EditDefaultsOnly)
	TArray<UClass*> PickupBPClasses;
	
	// Players to place in the grid at Restart
	UPROPERTY(EditInstanceOnly)
	TArray<ABomberPawn*> Players;

	// Starting Positions of the players in the grid (the grid corners)
	UPROPERTY()
	TArray<FIntPoint> StartingPositions;
	
	// Valid movement directions in the grid (North, South, East, West)
	UPROPERTY()
	TArray<FIntPoint> MovementDirections;

	UPROPERTY()
	FRandomStream RandomNumberGenerator;
};
