// Fill out your copyright notice in the Description page of Project Settings.

#include "Block.h"

#include "Engine.h"

// Sets default values
ABlock::ABlock()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABlock::BeginPlay()
{
	Super::BeginPlay();	
}

// Called every frame
void ABlock::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool ABlock::OnDamaged()
{
	if (!bIsBreakable)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("A Block was damaged but it is unbreakable"));
		}
		return false;
	}
	
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("A Block was destroyed!"));
	}
	RemoveFromGame();
	return true;
}

bool ABlock::RemoveFromGame()
{
	Destroy();
	return true;
}


bool ABlock::IsWalkable() const
{
	return false;
}
