// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CellOccupantInterface.h"
#include "Block.generated.h"

UCLASS()
class BOMBERMANTEST_API ABlock : public AActor, public ICellOccupantInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABlock();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual bool OnDamaged() override;
	virtual bool RemoveFromGame() override;

	UPROPERTY(EditAnywhere)
	bool bIsBreakable = false;
};
