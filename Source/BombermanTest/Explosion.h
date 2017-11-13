// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CellOccupantInterface.h"
#include "Explosion.generated.h"

class ALevelGrid;

// This represents a single Cell on fire
UCLASS()
class BOMBERMANTEST_API AExplosion : public AActor, public ICellOccupantInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AExplosion();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// This allows explosion effects to start triggering in their first tick instead of on spawn, to prevent recursion when bomb explosions reach other bombs
	bool bExistsInGrid = false;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// ICellOccupantInterface //
	virtual bool OnDamaged() override;
	virtual bool RemoveFromGame() override;
	virtual bool IsWalkable() const override;
	virtual void SetCurrentCell(FIntPoint Cell) override;
	virtual const FIntPoint GetCurrentCell() const override;
	// End ICellOccupantInterface //
	
	// How long does the explosion stay in the world
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ExplosionDuration = 1.0f;

	UPROPERTY()
	ALevelGrid* CurrentLevelGrid;

	UPROPERTY()
	FIntPoint CurrentCell;	


protected:
	UPROPERTY()
	float TimeToLive;

};
