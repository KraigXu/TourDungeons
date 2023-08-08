// Copyright 2020 Dan Kestranek.


#include "OCAbilitySystemComponent.h"

void UOCAbilitySystemComponent::ReceiveDamage(UOCAbilitySystemComponent* SourceASC, float UnmitigatedDamage, float MitigatedDamage)
{
	ReceivedDamage.Broadcast(SourceASC, UnmitigatedDamage, MitigatedDamage);
}
