// Copyright Epic Games, Inc. All Rights Reserved.

#include "FIPawn.h"

#include "GameFramework/Controller.h"
#include "Net/UnrealNetwork.h"
#include "UObject/ScriptInterface.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FIPawn)

class FLifetimeProperty;
class UObject;

AFIPawn::AFIPawn(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

// void AFIPawn::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
// {
// 	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
//
// 	DOREPLIFETIME(ThisClass, MyTeamID);
// }

void AFIPawn::PreInitializeComponents()
{
	Super::PreInitializeComponents();
}

void AFIPawn::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void AFIPawn::PossessedBy(AController* NewController)
{
	// const FGenericTeamId OldTeamID = MyTeamID;
	//
	// Super::PossessedBy(NewController);
	//
	// // Grab the current team ID and listen for future changes
	// if (ILyraTeamAgentInterface* ControllerAsTeamProvider = Cast<ILyraTeamAgentInterface>(NewController))
	// {
	// 	MyTeamID = ControllerAsTeamProvider->GetGenericTeamId();
	// 	ControllerAsTeamProvider->GetTeamChangedDelegateChecked().AddDynamic(this, &ThisClass::OnControllerChangedTeam);
	// }
	// ConditionalBroadcastTeamChanged(this, OldTeamID, MyTeamID);
}

void AFIPawn::UnPossessed()
{
	AController* const OldController = Controller;
	//stop listening for changes from the old controller

	// // Stop listening for changes from the old controller
	// const FGenericTeamId OldTeamID = MyTeamID;
	// if (ILyraTeamAgentInterface* ControllerAsTeamProvider = Cast<ILyraTeamAgentInterface>(OldController))
	// {
	// 	ControllerAsTeamProvider->GetTeamChangedDelegateChecked().RemoveAll(this);
	// }
	//
	// Super::UnPossessed();
	//
	// // Determine what the new team ID should be afterwards
	// MyTeamID = DetermineNewTeamAfterPossessionEnds(OldTeamID);
	// ConditionalBroadcastTeamChanged(this, OldTeamID, MyTeamID);
}

// void AFIPawn::SetGenericTeamId(const FGenericTeamId& NewTeamID)
// {
// 	if (GetController() == nullptr)
// 	{
// 		if (HasAuthority())
// 		{
// 			const FGenericTeamId OldTeamID = MyTeamID;
// 			MyTeamID = NewTeamID;
// 			ConditionalBroadcastTeamChanged(this, OldTeamID, MyTeamID);
// 		}
// 		else
// 		{
// 			UE_LOG(LogFITeams, Error, TEXT("You can't set the team ID on a pawn (%s) except on the authority"), *GetPathNameSafe(this));
// 		}
// 	}
// 	else
// 	{
// 		UE_LOG(LogFITeams, Error, TEXT("You can't set the team ID on a possessed pawn (%s); it's driven by the associated controller"), *GetPathNameSafe(this));
// 	}
// }
//
// FGenericTeamId AFIPawn::GetGenericTeamId() const
// {
// 	return MyTeamID;
// }
//
// FOnLyraTeamIndexChangedDelegate* AFIPawn::GetOnTeamIndexChangedDelegate()
// {
// 	return &OnTeamChangedDelegate;
// }
//
// void AFIPawn::OnControllerChangedTeam(UObject* TeamAgent, int32 OldTeam, int32 NewTeam)
// {
// 	const FGenericTeamId MyOldTeamID = MyTeamID;
// 	MyTeamID = IntegerToGenericTeamId(NewTeam);
// 	ConditionalBroadcastTeamChanged(this, MyOldTeamID, MyTeamID);
// }
//
// void AFIPawn::OnRep_MyTeamID(FGenericTeamId OldTeamID)
// {
// 	ConditionalBroadcastTeamChanged(this, OldTeamID, MyTeamID);
// }

