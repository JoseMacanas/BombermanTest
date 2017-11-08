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

	void EnterCell(ICellOccupantInterface* CellOccuppant, FVector2D Cell);
	void ExitCell(ICellOccupantInterface* CellOccuppant, FVector2D Cell);
	void ChangeCell(ICellOccupantInterface* CellOccuppant, FVector2D OldCell, FVector2D NewCell);

	void GenerateLevel(int RandomSeed = 0);


	FVector2D GetCellFromWorldCoordinates(FVector2D WorldCoordinates);
	FVector2D GetWorldCoordinatesFromCell(FVector2D Cell);
	
	UPROPERTY(EditInstanceOnly)
	float CellSize = 100;

	UPROPERTY(EditInstanceOnly)
	int GridWidth = 5;

	UPROPERTY(EditInstanceOnly)
	int GridHeight = 5;

	TMap<FVector2D, TSet<ICellOccupantInterface*>> CellOccupants;

	UPROPERTY(EditDefaultsOnly)
	UClass* SolidBlockBPClass;

	UPROPERTY(EditDefaultsOnly)
	UClass* BorderBlockBPClass;

	UPROPERTY(EditDefaultsOnly)
	UClass* BreakableBlockBPClass;
};
