#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "UObject/Interface.h"
#include "CellOccupantInterface.generated.h"

UINTERFACE(Blueprintable)
class UCellOccupantInterface : public UInterface
{
	GENERATED_BODY()
};

class ICellOccupantInterface
{
	GENERATED_BODY()

public:	
	virtual bool OnDamaged() = 0;
	virtual bool RemoveFromGame() = 0;
};
