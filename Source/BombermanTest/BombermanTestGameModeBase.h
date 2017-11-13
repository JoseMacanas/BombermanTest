// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BombermanTestGameModeBase.generated.h"

class ABomberPawn;
class ABombermanTestGameStateBase;
class ALevelGrid;

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
	
	// Gathers the players' scores and the winners of the round to call the Blueprint function OnGameEnd()
	void AddScoresAndEndGame(bool bIsTimeOut = false);
	
	// Implemented in the gamemode Blueprint, this function brings up the UI
	UFUNCTION(BlueprintImplementableEvent)
	bool OnGameEnd(const TArray<FString>& PlayerNames, const TArray<int>& Scores, const TArray<int>& RoundWinners);

	// Callback from UI to restart the game. Also called from code for the first round
	UFUNCTION(BlueprintCallable)
	bool OnGameRestart();

	UPROPERTY()
	ABombermanTestGameStateBase* CurrentGameState;

	UPROPERTY()
	ALevelGrid* CurrentLevelGrid;

	// How long will a round last
	UPROPERTY(EditAnywhere)
	int RoundTimeSeconds = 600;

	// Read by Blueprints to display the remaining round time on the UI
	UPROPERTY(BlueprintReadOnly)
	float RoundTimer = 0;
	
	// The names of the players in this round
	UPROPERTY()
	TArray<FString> PlayerNames;
};
