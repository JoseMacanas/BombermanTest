// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"




#include "LevelGrid.generated.h"

class ICellOccupantInterface;


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


	FIntPoint GetCellFromWorldCoordinates(FVector2D WorldCoordinates) const;
	FVector2D GetWorldCoordinatesFromCell(FIntPoint Cell) const;
	bool IsCellWalkable(FIntPoint Cell) const;

	
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
};
