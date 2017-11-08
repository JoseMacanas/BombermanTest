// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CellOccupantInterface.h"

class ALevelGrid;


#include "Bomb.generated.h"

UCLASS()
class BOMBERMANTEST_API ABomb : public AActor, public ICellOccupantInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABomb();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual bool OnDamaged() override;

	bool PlaceInWorld(ALevelGrid* OccupiedLevelGrid, FIntPoint Cell);

	void Explode();	

	bool bPlacedInWorld = false;
	
	float ExplodeTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ExplodeDelay = 3.0f;

	UPROPERTY()
	ALevelGrid* CurrentLevelGrid;

	UPROPERTY()
	FIntPoint CurrentCell;
};
