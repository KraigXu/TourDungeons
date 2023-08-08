// Copyright © 2021-2022 GDI. All Rights Reserved.

#include "OracleEquip/Public/OCEquipManager.h"


UOCEquipManager::UOCEquipManager()
{
}

void UOCEquipManager::BeginPlay()
{
	Super::BeginPlay();
	for (UActorComponent* Component : Cast<AActor>(this)->GetComponents())
	{
		if(UOCEquipPartComponent* EquipPartComponent=Cast<UOCEquipPartComponent>(Component))
		{
			EquipPartComponents.AddUnique(EquipPartComponent);	
		}
	}
}
