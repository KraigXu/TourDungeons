// Copyright Epic Games, Inc. All Rights Reserved.

#include "FIDungeonGameMode.h"
#include "AssetRegistry/AssetData.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "Misc/CommandLine.h"
// #include "System/LyraAssetManager.h"
#include "FIDungeonGameState.h"
// #include "System/LyraGameSession.h"
// #include "Player/LyraPlayerController.h"
// #include "Player/LyraPlayerBotController.h"
// #include "Player/LyraPlayerState.h"
// #include "Character/LyraCharacter.h"
// #include "UI/LyraHUD.h"
// #include "Character/LyraPawnExtensionComponent.h"
// #include "Character/LyraPawnData.h"
// #include "GameModes/LyraWorldSettings.h"
// #include "GameModes/LyraExperienceDefinition.h"
// #include "GameModes/LyraExperienceManagerComponent.h"
// #include "GameModes/LyraUserFacingExperienceDefinition.h"
#include "Kismet/GameplayStatics.h"
// #include "Development/LyraDeveloperSettings.h"
// #include "Player/LyraPlayerSpawningManagerComponent.h"
// #include "CommonUserSubsystem.h"
#include "CommonSessionSubsystem.h"
#include "FIDungeonModeManagerComponent.h"
#include "FIExperienceDefinition.h"
#include "TimerManager.h"
#include "GameMapsSettings.h"
#include "Character/FICharacter.h"
#include "Character/FIPawnData.h"
#include "Character/FIPawnExtensionComponent.h"
#include "Development/LyraDeveloperSettings.h"
#include "FateIronPro/FILogChannels.h"
#include "GameFramework/GameSession.h"
#include "Player/Dungenon/LyraPlayerController.h"
#include "Player/Dungenon/LyraPlayerState.h"
#include "System/FIAssetManager.h"
#include "UI/Dungeon/FIDungeonHUD.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FIDungeonGameMode)

AFIDungeonGameMode::AFIDungeonGameMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	GameStateClass = AFIDungeonGameState::StaticClass();
	GameSessionClass = AGameSession::StaticClass();
	PlayerControllerClass = ALyraPlayerController::StaticClass();
	ReplaySpectatorPlayerControllerClass = ALyraReplayPlayerController::StaticClass();
	PlayerStateClass = ALyraPlayerState::StaticClass();
	DefaultPawnClass = AFICharacter::StaticClass();
	HUDClass = AFIDungeonHUD::StaticClass();
}

const UFIPawnData* AFIDungeonGameMode::GetPawnDataForController(const AController* InController) const
{
	// See if pawn data is already set on the player state
	if (InController != nullptr)
	{
		if (const ALyraPlayerState* LyraPS = InController->GetPlayerState<ALyraPlayerState>())
		{
			if (const UFIPawnData* PawnData = LyraPS->GetPawnData<UFIPawnData>())
			{
				return PawnData;
			}
		}
	}

	// If not, fall back to the the default for the current experience
	// check(GameState);
	// ULyraExperienceManagerComponent* ExperienceComponent = GameState->FindComponentByClass<ULyraExperienceManagerComponent>();
	// check(ExperienceComponent);
	//
	// if (ExperienceComponent->IsExperienceLoaded())
	// {
	// 	const UFIExperienceDefinition* Experience = ExperienceComponent->GetCurrentExperienceChecked();
	// 	if (Experience->DefaultPawnData != nullptr)
	// 	{
	// 		return Experience->DefaultPawnData;
	// 	}
	//
	// 	// Experience is loaded and there's still no pawn data, fall back to the default for now
	// 	return ULyraAssetManager::Get().GetDefaultPawnData();
	// }

	// Experience not loaded yet, so there is no pawn data to be had
	return nullptr;
}

void AFIDungeonGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);
	
	// Wait for the next frame to give time to initialize startup settings
	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::HandleMatchAssignmentIfNotExpectingOne);
}

void AFIDungeonGameMode::HandleMatchAssignmentIfNotExpectingOne()
{
	FPrimaryAssetId ExperienceId;
	FString ExperienceIdSource;
	
	// Precedence order (highest wins)
	//  - Developer Settings (PIE only)
	//  - Command Line override
	//  - World Settings
	//  - Dedicated server
	//  - Default experience
	
	UWorld* World = GetWorld();
	if (!ExperienceId.IsValid() && UGameplayStatics::HasOption(OptionsString, TEXT("Experience")))
	{
		const FString ExperienceFromOptions = UGameplayStatics::ParseOption(OptionsString, TEXT("Experience"));
		ExperienceId = FPrimaryAssetId(FPrimaryAssetType(UFIExperienceDefinition::StaticClass()->GetFName()), FName(*ExperienceFromOptions));
		ExperienceIdSource = TEXT("OptionsString");
	}
	
	if (!ExperienceId.IsValid() && World->IsPlayInEditor())
	{
		ExperienceId = GetDefault<ULyraDeveloperSettings>()->ExperienceOverride;
		ExperienceIdSource = TEXT("DeveloperSettings");
	}
	
	// see if the command line wants to set the experience
	if (!ExperienceId.IsValid())
	{
		FString ExperienceFromCommandLine;
		UE_LOG(LogFIExperience,Log,TEXT("CommandLine= %s"),FCommandLine::Get());
		if (FParse::Value(FCommandLine::Get(), TEXT("Experience="), ExperienceFromCommandLine))
		{
			ExperienceId = FPrimaryAssetId::ParseTypeAndName(ExperienceFromCommandLine);
			if (!ExperienceId.PrimaryAssetType.IsValid())
			{
				ExperienceId = FPrimaryAssetId(FPrimaryAssetType(UFIExperienceDefinition::StaticClass()->GetFName()), FName(*ExperienceFromCommandLine));
			}
			ExperienceIdSource = TEXT("CommandLine");
		}
	}
	
	UFIAssetManager& AssetManager = UFIAssetManager::Get();
	FAssetData Dummy;
	if (ExperienceId.IsValid() && !AssetManager.GetPrimaryAssetData(ExperienceId, /*out*/ Dummy))
	{
		UE_LOG(LogFIExperience, Error, TEXT("EXPERIENCE: Wanted to use %s but couldn't find it, falling back to the default)"), *ExperienceId.ToString());
		ExperienceId = FPrimaryAssetId();
	}
	
	// Final fallback to the default experience
	if (!ExperienceId.IsValid())
	{
		if (TryDedicatedServerLogin())
		{
			// This will start to host as a dedicated server
			return;
		}
	
		//@TODO: Pull this from a config setting or something
		ExperienceId = FPrimaryAssetId(FPrimaryAssetType("FIExperienceDefinition"), FName("B_FIExperienceDefinition"));
		ExperienceIdSource = TEXT("Default");
	}
	
	OnMatchAssignmentGiven(ExperienceId, ExperienceIdSource);
}

bool AFIDungeonGameMode::TryDedicatedServerLogin()
{
	// // Some basic code to register as an active dedicated server, this would be heavily modified by the game
	// FString DefaultMap = UGameMapsSettings::GetGameDefaultMap();
	// UWorld* World = GetWorld();
	// UGameInstance* GameInstance = GetGameInstance();
	// if (GameInstance && World && World->GetNetMode() == NM_DedicatedServer && World->URL.Map == DefaultMap)
	// {
	// 	// Only register if this is the default map on a dedicated server
	// 	UCommonUserSubsystem* UserSubsystem = GameInstance->GetSubsystem<UCommonUserSubsystem>();
	//
	// 	// Dedicated servers may need to do an online login
	// 	UserSubsystem->OnUserInitializeComplete.AddDynamic(this, &AFIDungeonGameMode::OnUserInitializedForDedicatedServer);
	//
	// 	// There are no local users on dedicated server, but index 0 means the default platform user which is handled by the online login code
	// 	if (!UserSubsystem->TryToLoginForOnlinePlay(0))
	// 	{
	// 		OnUserInitializedForDedicatedServer(nullptr, false, FText(), ECommonUserPrivilege::CanPlayOnline, ECommonUserOnlineContext::Default);
	// 	}
	//
	// 	return true;
	// }
	return false;
}

void AFIDungeonGameMode::HostDedicatedServerMatch(ECommonSessionOnlineMode OnlineMode)
{
	 // FPrimaryAssetType UserExperienceType = ULyraUserFacingExperienceDefinition::StaticClass()->GetFName();
	 //
	 // // Figure out what UserFacingExperience to load
	 // FPrimaryAssetId UserExperienceId;
	 // FString UserExperienceFromCommandLine;
	 // if (FParse::Value(FCommandLine::Get(), TEXT("UserExperience="), UserExperienceFromCommandLine) ||
	 // 	FParse::Value(FCommandLine::Get(), TEXT("Playlist="), UserExperienceFromCommandLine))
	 // {
	 // 	UserExperienceId = FPrimaryAssetId::ParseTypeAndName(UserExperienceFromCommandLine);
	 // 	if (!UserExperienceId.PrimaryAssetType.IsValid())
	 // 	{
	 // 		UserExperienceId = FPrimaryAssetId(FPrimaryAssetType(UserExperienceType), FName(*UserExperienceFromCommandLine));
	 // 	}
	 // }

	// Search for the matching experience, it's fine to force load them because we're in dedicated server startup
	// ULyraAssetManager& AssetManager = ULyraAssetManager::Get();
	// TSharedPtr<FStreamableHandle> Handle = AssetManager.LoadPrimaryAssetsWithType(UserExperienceType);
	// if (ensure(Handle.IsValid()))
	// {
	// 	Handle->WaitUntilComplete();
	// }

	// TArray<UObject*> UserExperiences;
	// AssetManager.GetPrimaryAssetObjectList(UserExperienceType, UserExperiences);
	// ULyraUserFacingExperienceDefinition* FoundExperience = nullptr;
	// ULyraUserFacingExperienceDefinition* DefaultExperience = nullptr;
	//
	// for (UObject* Object : UserExperiences)
	// {
	// 	ULyraUserFacingExperienceDefinition* UserExperience = Cast<ULyraUserFacingExperienceDefinition>(Object);
	// 	if (ensure(UserExperience))
	// 	{
	// 		if (UserExperience->GetPrimaryAssetId() == UserExperienceId)
	// 		{
	// 			FoundExperience = UserExperience;
	// 			break;
	// 		}
	// 		
	// 		if (UserExperience->bIsDefaultExperience && DefaultExperience == nullptr)
	// 		{
	// 			DefaultExperience = UserExperience;
	// 		}
	// 	}
	// }

	// if (FoundExperience == nullptr)
	// {
	// 	FoundExperience = DefaultExperience;
	// }
	
	// UGameInstance* GameInstance = GetGameInstance();
	// if (ensure(FoundExperience && GameInstance))
	// {
	// 	// Actually host the game
	// 	// UCommonSession_HostSessionRequest* HostRequest = FoundExperience->CreateHostingRequest();
	// 	// if (ensure(HostRequest))
	// 	// {
	// 	// 	HostRequest->OnlineMode = OnlineMode;
	// 	//
	// 	// 	// TODO override other parameters?
	// 	//
	// 	// 	UCommonSessionSubsystem* SessionSubsystem = GameInstance->GetSubsystem<UCommonSessionSubsystem>();
	// 	// 	SessionSubsystem->HostSession(nullptr, HostRequest);
	// 	// 	
	// 	// 	// This will handle the map travel
	// 	// }
	// }
}


void AFIDungeonGameMode::OnUserInitializedForDedicatedServer(const UCommonUserInfo* UserInfo, bool bSuccess, FText Error, ECommonUserPrivilege RequestedPrivilege, ECommonUserOnlineContext OnlineContext)
{
	UGameInstance* GameInstance = GetGameInstance();
	if (GameInstance)
	{
		// Unbind
		UCommonUserSubsystem* UserSubsystem = GameInstance->GetSubsystem<UCommonUserSubsystem>();
		UserSubsystem->OnUserInitializeComplete.RemoveDynamic(this, &AFIDungeonGameMode::OnUserInitializedForDedicatedServer);
	
		if (bSuccess)
		{
			// Online login worked, start a full online game
			UE_LOG(LogFIExperience, Log, TEXT("Dedicated server online login succeeded, starting online server"));
			HostDedicatedServerMatch(ECommonSessionOnlineMode::Online);
		}
		else
		{
			// Go ahead and try to host anyway, but without online support
			// This behavior is fairly game specific, but this behavior provides the most flexibility for testing
			UE_LOG(LogFIExperience, Log, TEXT("Dedicated server online login failed, starting LAN-only server"));
			HostDedicatedServerMatch(ECommonSessionOnlineMode::LAN);
		}
	}
}

void AFIDungeonGameMode::OnMatchAssignmentGiven(FPrimaryAssetId ExperienceId, const FString& ExperienceIdSource)
{
	if (ExperienceId.IsValid())
	{
		UE_LOG(LogFIExperience, Log, TEXT("Identified experience %s (Source: %s)"), *ExperienceId.ToString(), *ExperienceIdSource);
	
		UFIDungeonModeManagerComponent* ExperienceComponent = GameState->FindComponentByClass<UFIDungeonModeManagerComponent>();
		check(ExperienceComponent);
		ExperienceComponent->SetCurrentExperience(ExperienceId);
	}
	else
	{
		UE_LOG(LogFI, Error, TEXT("Failed to identify experience, loading screen will stay up forever"));
	}
}

void AFIDungeonGameMode::OnExperienceLoaded(const UFIExperienceDefinition* CurrentExperience)
{
	// Spawn any players that are already attached
	//@TODO: Here we're handling only *player* controllers, but in GetDefaultPawnClassForController_Implementation we skipped all controllers
	// GetDefaultPawnClassForController_Implementation might only be getting called for players anyways
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* PC = Cast<APlayerController>(*Iterator);
		if ((PC != nullptr) && (PC->GetPawn() == nullptr))
		{
			if (PlayerCanRestart(PC))
			{
				RestartPlayer(PC);
			}
		}
	}
}

bool AFIDungeonGameMode::IsExperienceLoaded() const
{
	check(GameState);
	UFIDungeonModeManagerComponent* ExperienceComponent = GameState->FindComponentByClass<UFIDungeonModeManagerComponent>();
	check(ExperienceComponent);

	return ExperienceComponent->IsExperienceLoaded();
}

UClass* AFIDungeonGameMode::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	 if (const UFIPawnData* PawnData = GetPawnDataForController(InController))
	 {
	 	if (PawnData->PawnClass)
	 	{
	 		return PawnData->PawnClass;
	 	}
	 }

	return Super::GetDefaultPawnClassForController_Implementation(InController);
}

APawn* AFIDungeonGameMode::SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer, const FTransform& SpawnTransform)
{
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.Instigator = GetInstigator();
	SpawnInfo.ObjectFlags |= RF_Transient;	// Never save the default player pawns into a map.
	SpawnInfo.bDeferConstruction = true;

	if (UClass* PawnClass = GetDefaultPawnClassForController(NewPlayer))
	{
		if (APawn* SpawnedPawn = GetWorld()->SpawnActor<APawn>(PawnClass, SpawnTransform, SpawnInfo))
		{
			if (UFIPawnExtensionComponent* PawnExtComp = UFIPawnExtensionComponent::FindPawnExtensionComponent(SpawnedPawn))
			{
				if (const UFIPawnData* PawnData = GetPawnDataForController(NewPlayer))
				{
					PawnExtComp->SetPawnData(PawnData);
				}
				else
				{
					UE_LOG(LogFI, Error, TEXT("Game mode was unable to set PawnData on the spawned pawn [%s]."), *GetNameSafe(SpawnedPawn));
				}
			}
			
			SpawnedPawn->FinishSpawning(SpawnTransform);
			return SpawnedPawn;
		}
		else
		{
			UE_LOG(LogFI, Error, TEXT("Game mode was unable to spawn Pawn of class [%s] at [%s]."), *GetNameSafe(PawnClass), *SpawnTransform.ToHumanReadableString());
		}
	}
	else
	{
		UE_LOG(LogFI, Error, TEXT("Game mode was unable to spawn Pawn due to NULL pawn class."));
	}

	return nullptr;
}

bool AFIDungeonGameMode::ShouldSpawnAtStartSpot(AController* Player)
{
	// We never want to use the start spot, always use the spawn management component.
	return false;
}

void AFIDungeonGameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	// Delay starting new players until the experience has been loaded
	// (players who log in prior to that will be started by OnExperienceLoaded)
	if (IsExperienceLoaded())
	{
		Super::HandleStartingNewPlayer_Implementation(NewPlayer);
	}
}

AActor* AFIDungeonGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	// if (ULyraPlayerSpawningManagerComponent* PlayerSpawningComponent = GameState->FindComponentByClass<ULyraPlayerSpawningManagerComponent>())
	// {
	// 	return PlayerSpawningComponent->ChoosePlayerStart(Player);
	// }
	
	return Super::ChoosePlayerStart_Implementation(Player);
}

void AFIDungeonGameMode::FinishRestartPlayer(AController* NewPlayer, const FRotator& StartRotation)
{
	// if (ULyraPlayerSpawningManagerComponent* PlayerSpawningComponent = GameState->FindComponentByClass<ULyraPlayerSpawningManagerComponent>())
	// {
	// 	PlayerSpawningComponent->FinishRestartPlayer(NewPlayer, StartRotation);
	// }

	Super::FinishRestartPlayer(NewPlayer, StartRotation);
}

bool AFIDungeonGameMode::PlayerCanRestart_Implementation(APlayerController* Player)
{
	return ControllerCanRestart(Player);
}

bool AFIDungeonGameMode::ControllerCanRestart(AController* Controller)
{
	if (APlayerController* PC = Cast<APlayerController>(Controller))
	{	
		if (!Super::PlayerCanRestart_Implementation(PC))
		{
			return false;
		}
	}
	else
	{
		// Bot version of Super::PlayerCanRestart_Implementation
		if ((Controller == nullptr) || Controller->IsPendingKillPending())
		{
			return false;
		}
	}

	// if (ULyraPlayerSpawningManagerComponent* PlayerSpawningComponent = GameState->FindComponentByClass<ULyraPlayerSpawningManagerComponent>())
	// {
	// 	return PlayerSpawningComponent->ControllerCanRestart(Controller);
	// }

	return true;
}

void AFIDungeonGameMode::InitGameState()
{
	Super::InitGameState();

	// Listen for the experience load to complete	
	//ULyraExperienceManagerComponent* ExperienceComponent = GameState->FindComponentByClass<ULyraExperienceManagerComponent>();
	//check(ExperienceComponent);
	//ExperienceComponent->CallOrRegister_OnExperienceLoaded(FOnLyraExperienceLoaded::FDelegate::CreateUObject(this, &ThisClass::OnExperienceLoaded));
}

void AFIDungeonGameMode::GenericPlayerInitialization(AController* NewPlayer)
{
	Super::GenericPlayerInitialization(NewPlayer);

	OnGameModePlayerInitialized.Broadcast(this, NewPlayer);
}

void AFIDungeonGameMode::RequestPlayerRestartNextFrame(AController* Controller, bool bForceReset)
{
	if (bForceReset && (Controller != nullptr))
	{
		Controller->Reset();
	}

	// if (APlayerController* PC = Cast<APlayerController>(Controller))
	// {
	// 	GetWorldTimerManager().SetTimerForNextTick(PC, &APlayerController::ServerRestartPlayer_Implementation);
	// }
	// else if (ALyraPlayerBotController* BotController = Cast<ALyraPlayerBotController>(Controller))
	// {
	// 	GetWorldTimerManager().SetTimerForNextTick(BotController, &ALyraPlayerBotController::ServerRestartController);
	// }
}

bool AFIDungeonGameMode::UpdatePlayerStartSpot(AController* Player, const FString& Portal, FString& OutErrorMessage)
{
	// Do nothing, we'll wait until PostLogin when we try to spawn the player for real.
	// Doing anything right now is no good, systems like team assignment haven't even occurred yet.
	return true;
}

void AFIDungeonGameMode::FailedToRestartPlayer(AController* NewPlayer)
{
	Super::FailedToRestartPlayer(NewPlayer);

	// If we tried to spawn a pawn and it failed, lets try again *note* check if there's actually a pawn class
	// before we try this forever.
	if (UClass* PawnClass = GetDefaultPawnClassForController(NewPlayer))
	{
		if (APlayerController* NewPC = Cast<APlayerController>(NewPlayer))
		{
			// If it's a player don't loop forever, maybe something changed and they can no longer restart if so stop trying.
			if (PlayerCanRestart(NewPC))
			{
				RequestPlayerRestartNextFrame(NewPlayer, false);			
			}
			else
			{
				UE_LOG(LogFI, Verbose, TEXT("FailedToRestartPlayer(%s) and PlayerCanRestart returned false, so we're not going to try again."), *GetPathNameSafe(NewPlayer));
			}
		}
		else
		{
			RequestPlayerRestartNextFrame(NewPlayer, false);
		}
	}
	else
	{
		UE_LOG(LogFI, Verbose, TEXT("FailedToRestartPlayer(%s) but there's no pawn class so giving up."), *GetPathNameSafe(NewPlayer));
	}
}
