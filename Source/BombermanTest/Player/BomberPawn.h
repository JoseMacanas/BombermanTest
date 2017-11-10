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
	
	void MoveXAxis(float AxisValue);
	void MoveYAxis(float AxisValue);
	void PlaceBomb();

	bool bIsAlive = true;


	FVector CurrentVelocity;

	UPROPERTY(EditInstanceOnly)
	float CurrentSpeed = 0;

	UPROPERTY(EditInstanceOnly)
	int PlayerId = 0;

	UPROPERTY(EditInstanceOnly)
	FIntPoint StartingCell = FIntPoint(0,0);

	UPROPERTY(EditDefaultsOnly)
	UClass* BombBPClass;

	UPROPERTY(EditDefaultsOnly)
	int MaxBombs;
	
	UPROPERTY()
	TArray<ABomb*> BombPool;
	
	UPROPERTY(EditInstanceOnly)
	ALevelGrid* CurrentLevelGrid;

	UPROPERTY()
	FIntPoint CurrentCell;

	UPROPERTY(EditInstanceOnly)
	FString PlayerName;
};
