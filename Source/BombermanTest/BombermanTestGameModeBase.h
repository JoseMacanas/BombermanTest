// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"

class ABomberPawn;

#include "BombermanTestGameModeBase.generated.h"


/**
 * 
 */
UCLASS()
class BOMBERMANTEST_API ABombermanTestGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	ABombermanTestGameModeBase();
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintImplementableEvent)
	bool OnGameEnd();

	UFUNCTION(BlueprintCallable)
	bool OnGameRestart();

	UPROPERTY()
	TArray<ABomberPawn*> Players;
};
