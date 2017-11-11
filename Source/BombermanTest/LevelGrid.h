// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

// This can't be forward declared because it needs to be exposed to the blueprint (UPROPERTY(EditInstanceOnly))
#include "Player/BomberPawn.h"

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

	void EnterCell(ICellOccupantInterface* CellOccuppant, FIntPoint Cell);
	void ExitCell(ICellOccupantInterface* CellOccuppant, FIntPoint Cell);
	void ChangeCell(ICellOccupantInterface* CellOccuppant, FIntPoint OldCell, FIntPoint NewCell);

	void GenerateLevel(int RandomSeed = 0);
	void PlacePlayers();
	
	void RestartGame();

	TArray<FString> GetPlayerNames();

	FIntPoint GetCellFromWorldCoordinates(FVector2D WorldCoordinates) const;
	FVector2D GetWorldCoordinatesFromCell(FIntPoint Cell) const;
	bool IsCellWalkable(FIntPoint Cell) const;

	void SpawnRandomPickup(FIntPoint Cell);

	void SpawnChainReaction(ABomb* Bomb);
	void SpawnExplosion(ABomb* Bomb, TArray<ABomb*>& AffectedBombs, TArray<int>& AffectedPlayers);

	// Returns true if the fire goes through (it isn't blocked), and false if the explosion fire is blocked at this location
	bool SpawnExplosionFire(UWorld* const World, FActorSpawnParameters SpawnParams, FIntPoint Cell, TArray<ABomb*>& AffectedBombs, TArray<int>& AffectedPlayers);

	

	void SetStartingPositions();
	void SetMovementDirections();
	
	UPROPERTY(EditInstanceOnly)
	float CellSize = 100;

	UPROPERTY(EditInstanceOnly)
	int GridWidth = 5;

	UPROPERTY(EditInstanceOnly)
	int GridHeight = 5;

	TMap<FIntPoint, TSet<ICellOccupantInterface*>> CellOccupants;

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
	
	UPROPERTY(EditInstanceOnly)
	TArray<ABomberPawn*> Players;

	TArray<FIntPoint> StartingPositions;
	TArray<FIntPoint> MovementDirections;

	FRandomStream RandomNumberGenerator;
};
