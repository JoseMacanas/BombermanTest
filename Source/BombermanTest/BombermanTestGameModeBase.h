// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"

class ABomberPawn;
class ABombermanTestGameStateBase;
class ALevelGrid;

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
	bool OnGameEnd(const TArray<FString>& PlayerNames, const TArray<int>& Scores, const TArray<int>& RoundWinners);

	void AddScoresAndEndGame();

	UFUNCTION(BlueprintCallable)
	bool OnGameRestart();

	ABombermanTestGameStateBase* CurrentGameState;
	ALevelGrid* CurrentLevelGrid;

	UPROPERTY(EditAnywhere)
	int RoundTimeSeconds = 600;

	float RoundTimer = 0;

	TArray<FString> PlayerNames;
};
