// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CellOccupantInterface.h"
#include "Pickup.generated.h"

class ALevelGrid;
class ABomberPawn;

UCLASS()
class BOMBERMANTEST_API APickup : public AActor, public ICellOccupantInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APickup();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void ApplyEffects(ABomberPawn* Player);

	// ICellOccupantInterface //
	virtual bool OnDamaged() override;
	virtual bool RemoveFromGame() override;
	virtual bool IsWalkable() const override;
	virtual void SetCurrentCell(FIntPoint Cell) override;
	virtual const FIntPoint GetCurrentCell() const override;
	// End ICellOccupantInterface //
			
	UPROPERTY()
	ALevelGrid* CurrentLevelGrid;

	UPROPERTY()
	FIntPoint CurrentCell;
};
