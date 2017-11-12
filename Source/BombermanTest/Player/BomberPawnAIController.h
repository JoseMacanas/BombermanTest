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

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	virtual void Possess(APawn* InPawn) override;

	FIntPoint GetRandomWalkingDirection(bool bAvoidCollisions);

	ABomberPawn* BomberPawn;
	ALevelGrid* CurrentLevelGrid;

	FIntPoint TargetCell;

	FRandomStream RandomNumberGenerator;

	FIntPoint WalkingDirection;
};
