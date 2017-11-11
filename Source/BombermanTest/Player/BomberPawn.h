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

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	virtual bool OnDamaged() override;
	virtual bool RemoveFromGame() override;
	virtual bool IsWalkable() const override;

	void Reset();
	void Move(float DeltaTime);

	void PlaceInGrid(ALevelGrid* LevelGrid, FIntPoint StartingCell, int PlayerIndex);

	void MoveXAxis(float AxisValue);
	void MoveYAxis(float AxisValue);
	void PlaceBomb();
	void ReleaseBomb();

	void IncreaseAvailableBombs(int Increase);
	void IncreaseExplosionRange(int Increase);
	void IncreaseSpeed(float Increase);
	void IncreaseRemoteBombsTime(float Increase);

	
	bool bIsAlive = false;
	
	FVector CurrentVelocity;

	UPROPERTY()
	int PlayerId = 0;

	//////////////////
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

	//////////////////
	UPROPERTY(EditDefaultsOnly)
	int MaxSpeedIncreases = 5;

	UPROPERTY(EditDefaultsOnly)
	int MaxBombs = 10;

	UPROPERTY(EditDefaultsOnly)
	int MaxExplosionRange = 10;
	//////////////////
	
	//////////////////
	UPROPERTY(EditDefaultsOnly)
	int StartingBombs = 1;

	UPROPERTY(EditDefaultsOnly)
	int StartingExplosionRange = 2;

	UPROPERTY(EditDefaultsOnly)
	int StartingSpeed = 200;
	//////////////////


	UPROPERTY()
	int PlacedBombs;
	
	UPROPERTY()
	TArray<ABomb*> BombPool;
	
	UPROPERTY()
	ALevelGrid* CurrentLevelGrid;

	UPROPERTY()
	FIntPoint CurrentCell;

	UPROPERTY(EditInstanceOnly)
	FString PlayerName;

	UPROPERTY(EditDefaultsOnly)
	UClass* BombBPClass;
};
