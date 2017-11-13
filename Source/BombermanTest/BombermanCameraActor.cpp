// Fill out your copyright notice in the Description page of Project Settings.

#include "BombermanCameraActor.h"
#include "LevelGrid.h"
#include "Camera/CameraComponent.h"
#include "Engine.h"


// Sets default values
ABombermanCameraActor::ABombermanCameraActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}


// Called when the game starts or when spawned
void ABombermanCameraActor::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void ABombermanCameraActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
		
	UCameraComponent* CameraComponent = GetCameraComponent();
	float FoV = FMath::DegreesToRadians(CameraComponent->FieldOfView);

	FVector CurrentLocation = GetActorLocation();

	if (LevelGrid)
	{
		FVector CenterOfInterest;
		FVector2D SizeOfInterest;
			
		LevelGrid->GetCurrentAreaOfInterest(CenterOfInterest, SizeOfInterest);

		float Distance = CenterOfInterest.Z + FMath::Atan(FoV)*((SizeOfInterest.X)*0.5f);
		FVector TargetLocation = CenterOfInterest + FVector(0, 0, Distance);
		
		SetActorLocation(TargetLocation);
	}
}