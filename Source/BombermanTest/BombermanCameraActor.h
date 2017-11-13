// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraActor.h"

#include "BombermanCameraActor.generated.h"

class ALevelGrid;

/**
 * 
 */
UCLASS()
class BOMBERMANTEST_API ABombermanCameraActor : public ACameraActor
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ABombermanCameraActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	bool bIsCameraCentered = false;
	UPROPERTY(EditInstanceOnly)
	ALevelGrid* LevelGrid;	
};
