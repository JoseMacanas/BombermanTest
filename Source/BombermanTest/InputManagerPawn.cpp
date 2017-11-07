// Fill out your copyright notice in the Description page of Project Settings.

#include "InputManagerPawn.h"


#include "Engine.h"
#include "Player/BomberPawn.h"

#include "Runtime/Engine/Classes/Components/InputComponent.h"
#include "Runtime/Engine/Classes/Engine/World.h"


// Sets default values
AInputManagerPawn::AInputManagerPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AInputManagerPawn::BeginPlay()
{
	Super::BeginPlay();	
}

// Called every frame
void AInputManagerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AInputManagerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (Players.Num() > 0 && Players[0])
	{
		PlayerInputComponent->BindAction("P1_PlaceBomb", IE_Pressed, this, &AInputManagerPawn::P1_PlaceBomb);
		PlayerInputComponent->BindAxis("P1_XAxis", this, &AInputManagerPawn::P1_MoveXAxis);
		PlayerInputComponent->BindAxis("P1_YAxis", this, &AInputManagerPawn::P1_MoveYAxis);
	}
	if (Players.Num() > 1 && Players[1])
	{
		PlayerInputComponent->BindAction("P2_PlaceBomb", IE_Pressed, this, &AInputManagerPawn::P2_PlaceBomb);
		PlayerInputComponent->BindAxis("P2_XAxis", this, &AInputManagerPawn::P2_MoveXAxis);
		PlayerInputComponent->BindAxis("P2_YAxis", this, &AInputManagerPawn::P2_MoveYAxis);
	}
}

void AInputManagerPawn::P1_PlaceBomb()
{
	if (Players.Num() > 0 && Players[0])
	{
		Players[0]->PlaceBomb();
	}
}

void AInputManagerPawn::P1_MoveXAxis(float AxisValue)
{
	if (Players.Num() > 0 && Players[0])
	{
		Players[0]->MoveXAxis(AxisValue);
	}
}

void AInputManagerPawn::P1_MoveYAxis(float AxisValue)
{
	if (Players.Num() > 0 && Players[0])
	{
		Players[0]->MoveYAxis(AxisValue);
	}
}

void AInputManagerPawn::P2_PlaceBomb()
{
	if (Players.Num() > 1 && Players[1])
	{
		Players[1]->PlaceBomb();
	}
}

void AInputManagerPawn::P2_MoveXAxis(float AxisValue)
{
	if (Players.Num() > 1 && Players[1])
	{
		Players[1]->MoveXAxis(AxisValue);
	}
}

void AInputManagerPawn::P2_MoveYAxis(float AxisValue)
{
	if (Players.Num() > 1 && Players[1])
	{
		Players[1]->MoveYAxis(AxisValue);
	}
}


