// Fill out your copyright notice in the Description page of Project Settings.

#include "Explosion.h"
#include "LevelGrid.h"

// Sets default values
AExplosion::AExplosion()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AExplosion::BeginPlay()
{
	Super::BeginPlay();

	TimeToLive = ExplosionDuration;	
}

// Called every frame
void AExplosion::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bExistsInGrid)
	{
		bExistsInGrid = true;
		if (CurrentLevelGrid)
		{
			CurrentLevelGrid->EnterCell(this, CurrentCell);
		}
	}

	TimeToLive -= DeltaTime;

	if (TimeToLive <= 0)
	{
		RemoveFromGame();
	}
}

void AExplosion::RemoveFromGame()
{
	if (CurrentLevelGrid)
	{
		CurrentLevelGrid->ExitCell(this, CurrentCell);
	}

	Destroy();
}

bool AExplosion::OnDamaged()
{
	return false;
}
