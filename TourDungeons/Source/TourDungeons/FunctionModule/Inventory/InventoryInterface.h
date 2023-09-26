#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InventoryInterface.generated.h"

UINTERFACE(MinimalAPI)
class UInventoryInterface : public UInterface
{
    GENERATED_BODY()
};

class IInventoryInterface
{
    GENERATED_BODY()
public:
    virtual void GetContainerProperties();

    virtual bool GetCanStoreItems();

    virtual TArray<APlayerState*> GetPlayersViewing();
};
