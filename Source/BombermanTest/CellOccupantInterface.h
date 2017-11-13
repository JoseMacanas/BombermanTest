#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "UObject/Interface.h"
#include "CellOccupantInterface.generated.h"


// This is implemented as an interface instead of a class because I didn't want to force a common hierarchy on classes as different as a BomberPawn and a Pickup
// but Unreal doesn't seem to support multiple inheritance on UCLASS classes
UINTERFACE(Blueprintable)
class UCellOccupantInterface : public UInterface
{
	GENERATED_BODY()
};

class ICellOccupantInterface
{
	GENERATED_BODY()

public:
	// Called when a CellOccupant is damaged by an explosion
	virtual bool OnDamaged() = 0;

	// Called when a CellOccupant is removed from the grid
	virtual bool RemoveFromGame() = 0;

	// True if the CellOccupant doesn't block player movement
	virtual bool IsWalkable() const = 0;

	virtual void SetCurrentCell(FIntPoint Cell) = 0;
	virtual const FIntPoint GetCurrentCell() const = 0;
};
