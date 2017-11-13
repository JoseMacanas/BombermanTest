// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "InputManagerPawn.generated.h"

class ABomberPawn;


// This Pawn is required to re-route a single input device (keyboard) to two different Pawns (Player1 and Player2)
UCLASS()
class BOMBERMANTEST_API AInputManagerPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AInputManagerPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<ABomberPawn*> Players;
	
	void P1_PlaceBomb();
	void P1_MoveXAxis(float AxisValue);
	void P1_MoveYAxis(float AxisValue);

	void P2_PlaceBomb();
	void P2_MoveXAxis(float AxisValue);
	void P2_MoveYAxis(float AxisValue);

};
