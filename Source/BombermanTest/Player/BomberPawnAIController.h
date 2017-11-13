// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BomberPawnAIController.generated.h"

class ABomberPawn;
class ALevelGrid;
/**
 * 
 */
UCLASS()
class BOMBERMANTEST_API ABomberPawnAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	// Sets default values for this pawn's properties
	ABomberPawnAIController();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Check the Grid for any dangers (Bombs or Explosions) and set WalkingDirection to avoid them
	bool AvoidDangers();

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	// Called when the AI Controller takes ownership of the Pawn
	virtual void Possess(APawn* InPawn) override;

	// Provides a random walking direction in the form of a 2D int point with values of -1 or 1 for each coordinate
	// bAvoidCollisions discards walking directions that lead into an obstacle and then selects a random direction from the remaining posibilities
	FIntPoint GetRandomWalkingDirection(bool bAvoidCollisions);

	// Possessed Pawn
	UPROPERTY()
	ABomberPawn* BomberPawn;

	// Grid on which the Possessed Pawn is placed
	UPROPERTY()
	ALevelGrid* CurrentLevelGrid;
	
	UPROPERTY()
	FRandomStream RandomNumberGenerator;

	// This will become the input for the pawn every tick
	UPROPERTY()
	FIntPoint WalkingDirection;
};
