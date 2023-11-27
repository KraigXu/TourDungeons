// Copyright Epic Games, Inc. All Rights Reserved.

#include "FIHeroComponent.h"
#include "Components/GameFrameworkComponentDelegates.h"
#include "Logging/MessageLog.h"
// #include "Input/LyraMappableConfigPair.h"

#include "EnhancedInputSubsystems.h"
#include "FICharacter.h"
#include "FIPawnData.h"
#include "FIPawnExtensionComponent.h"
// #include "Player/LyraPlayerController.h"
// #include "Player/LyraPlayerState.h"
// #include "Player/LyraLocalPlayer.h"


#include "Components/GameFrameworkComponentManager.h"
#include "PlayerMappableInputConfig.h"
//#include "Camera/LyraCameraMode.h"
#include "UserSettings/EnhancedInputUserSettings.h"
#include "InputMappingContext.h"
#include "FateIronPro/FIGameplayTags.h"
#include "FateIronPro/FILogChannels.h"
#include "FateIronPro/AbilitySystem/FIAbilitySystemComponent.h"
#include "FateIronPro/Camera/FICameraComponent.h"
#include "FateIronPro/Input/FIInputComponent.h"
#include "FateIronPro/System/FIAssetManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FIHeroComponent)

#if WITH_EDITOR
#include "Misc/UObjectToken.h"
#endif	// WITH_EDITOR

namespace LyraHero
{
	static const float LookYawRate = 300.0f;
	static const float LookPitchRate = 165.0f;
};

const FName UFIHeroComponent::NAME_BindInputsNow("BindInputsNow");
const FName UFIHeroComponent::NAME_ActorFeatureName("Hero");

UFIHeroComponent::UFIHeroComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	//AbilityCameraMode = nullptr;
	bReadyToBindInputs = false;
}

void UFIHeroComponent::OnRegister()
{
	Super::OnRegister();

	if (!GetPawn<APawn>())
	{
		UE_LOG(LogFI, Error, TEXT("[UFIHeroComponent::OnRegister] This component has been added to a blueprint whose base class is not a Pawn. To use this component, it MUST be placed on a Pawn Blueprint."));

#if WITH_EDITOR
		if (GIsEditor)
		{
			static const FText Message = NSLOCTEXT("LyraHeroComponent", "NotOnPawnError", "has been added to a blueprint whose base class is not a Pawn. To use this component, it MUST be placed on a Pawn Blueprint. This will cause a crash if you PIE!");
			static const FName HeroMessageLogName = TEXT("LyraHeroComponent");
			
			FMessageLog(HeroMessageLogName).Error()
				->AddToken(FUObjectToken::Create(this, FText::FromString(GetNameSafe(this))))
				->AddToken(FTextToken::Create(Message));
				
			FMessageLog(HeroMessageLogName).Open();
		}
#endif
	}
	else
	{
		// Register with the init state system early, this will only work if this is a game world
		RegisterInitStateFeature();
	}
}

bool UFIHeroComponent::CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const
{
	check(Manager);

	APawn* Pawn = GetPawn<APawn>();

	if (!CurrentState.IsValid() && DesiredState == FIGameplayTags::InitState_Spawned)
	{
		// As long as we have a real pawn, let us transition
		if (Pawn)
		{
			return true;
		}
	}
	else if (CurrentState == FIGameplayTags::InitState_Spawned && DesiredState == FIGameplayTags::InitState_DataAvailable)
	{
		// The player state is required.
		// if (!GetPlayerState<ALyraPlayerState>())
		// {
		// 	return false;
		// }

		// If we're authority or autonomous, we need to wait for a controller with registered ownership of the player state.
		if (Pawn->GetLocalRole() != ROLE_SimulatedProxy)
		{
			AController* Controller = GetController<AController>();
			//
			// const bool bHasControllerPairedWithPS = (Controller != nullptr) && \
			// 	(Controller->PlayerState != nullptr) && \
			// 	(Controller->PlayerState->GetOwner() == Controller);
			//
			// if (!bHasControllerPairedWithPS)
			// {
			// 	return false;
			// }
		}

		const bool bIsLocallyControlled = Pawn->IsLocallyControlled();
		const bool bIsBot = Pawn->IsBotControlled();

		if (bIsLocallyControlled && !bIsBot)
		{
			//ALyraPlayerController* LyraPC = GetController<ALyraPlayerController>();

			// The input component and local player is required when locally controlled.
			// if (!Pawn->InputComponent || !LyraPC || !LyraPC->GetLocalPlayer())
			// {
			// 	return false;
			// }
		}

		return true;
	}
	else if (CurrentState == FIGameplayTags::InitState_DataAvailable && DesiredState == FIGameplayTags::InitState_DataInitialized)
	{
		// Wait for player state and extension component
		//ALyraPlayerState* LyraPS = GetPlayerState<ALyraPlayerState>();

		//return LyraPS && Manager->HasFeatureReachedInitState(Pawn, UFIPawnExtensionComponent::NAME_ActorFeatureName, FIGameplayTags::InitState_DataInitialized);
	}
	else if (CurrentState == FIGameplayTags::InitState_DataInitialized && DesiredState == FIGameplayTags::InitState_GameplayReady)
	{
		// TODO add ability initialization checks?
		return true;
	}

	return false;
}

void UFIHeroComponent::HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState)
{
	if (CurrentState == FIGameplayTags::InitState_DataAvailable && DesiredState == FIGameplayTags::InitState_DataInitialized)
	{
		APawn* Pawn = GetPawn<APawn>();
		// ALyraPlayerState* LyraPS = GetPlayerState<ALyraPlayerState>();
		// if (!ensure(Pawn && LyraPS))
		// {
		// 	return;
		// }
		//
		// const UFIPawnData* PawnData = nullptr;
		//
		// if (UFIPawnExtensionComponent* PawnExtComp = UFIPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
		// {
		// 	PawnData = PawnExtComp->GetPawnData<UFIPawnData>();
		//
		// 	// The player state holds the persistent data for this player (state that persists across deaths and multiple pawns).
		// 	// The ability system component and attribute sets live on the player state.
		// 	//PawnExtComp->InitializeAbilitySystem(LyraPS->GetLyraAbilitySystemComponent(), LyraPS);
		// }

		// if (ALyraPlayerController* LyraPC = GetController<ALyraPlayerController>())
		// {
		// 	if (Pawn->InputComponent != nullptr)
		// 	{
		// 		InitializePlayerInput(Pawn->InputComponent);
		// 	}
		// }

		// Hook up the delegate for all pawns, in case we spectate later
		// if (PawnData)
		// {
		// 	if (UFICameraComponent* CameraComponent = UFICameraComponent::FindCameraComponent(Pawn))
		// 	{
		// 		//CameraComponent->DetermineCameraModeDelegate.BindUObject(this, &ThisClass::DetermineCameraMode);
		// 	}
		// }
	}
}

void UFIHeroComponent::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)
{
	if (Params.FeatureName == UFIPawnExtensionComponent::NAME_ActorFeatureName)
	{
		if (Params.FeatureState == FIGameplayTags::InitState_DataInitialized)
		{
			// If the extension component says all all other components are initialized, try to progress to next state
			CheckDefaultInitialization();
		}
	}
}

void UFIHeroComponent::CheckDefaultInitialization()
{
	static const TArray<FGameplayTag> StateChain = { FIGameplayTags::InitState_Spawned, FIGameplayTags::InitState_DataAvailable, FIGameplayTags::InitState_DataInitialized, FIGameplayTags::InitState_GameplayReady };

	// This will try to progress from spawned (which is only set in BeginPlay) through the data initialization stages until it gets to gameplay ready
	ContinueInitStateChain(StateChain);
}

void UFIHeroComponent::BeginPlay()
{
	Super::BeginPlay();

	// Listen for when the pawn extension component changes init state
	BindOnActorInitStateChanged(UFIPawnExtensionComponent::NAME_ActorFeatureName, FGameplayTag(), false);

	// Notifies that we are done spawning, then try the rest of initialization
	ensure(TryToChangeInitState(FIGameplayTags::InitState_Spawned));
	CheckDefaultInitialization();
}

void UFIHeroComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnregisterInitStateFeature();

	Super::EndPlay(EndPlayReason);
}
//
// void UFIHeroComponent::InitializePlayerInput(UInputComponent* PlayerInputComponent)
// {
// 	check(PlayerInputComponent);
//
// 	const APawn* Pawn = GetPawn<APawn>();
// 	if (!Pawn)
// 	{
// 		return;
// 	}
//
// 	const APlayerController* PC = GetController<APlayerController>();
// 	check(PC);
//
// 	// const ULyraLocalPlayer* LP = Cast<ULyraLocalPlayer>(PC->GetLocalPlayer());
// 	// check(LP);
// 	//
// 	// UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
// 	// check(Subsystem);
// 	//
// 	// Subsystem->ClearAllMappings();
//
// 	if (const UFIPawnExtensionComponent* PawnExtComp = UFIPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
// 	{
// 		if (const UFIPawnData* PawnData = PawnExtComp->GetPawnData<UFIPawnData>())
// 		{
// 			if (const UFIInputConfig* InputConfig = PawnData->InputConfig)
// 			{
// 				// for (const FInputMappingContextAndPriority& Mapping : DefaultInputMappings)
// 				// {
// 				// 	if (UInputMappingContext* IMC = Mapping.InputMapping.Get())
// 				// 	{
// 				// 		if (Mapping.bRegisterWithSettings)
// 				// 		{
// 				// 			if (UEnhancedInputUserSettings* Settings = Subsystem->GetUserSettings())
// 				// 			{
// 				// 				Settings->RegisterInputMappingContext(IMC);
// 				// 			}
// 				// 			
// 				// 			FModifyContextOptions Options = {};
// 				// 			Options.bIgnoreAllPressedKeysUntilRelease = false;
// 				// 			// Actually add the config to the local player							
// 				// 			Subsystem->AddMappingContext(IMC, Mapping.Priority, Options);
// 				// 		}
// 				// 	}
// 				// }
//
// 				// The Lyra Input Component has some additional functions to map Gameplay Tags to an Input Action.
// 				// If you want this functionality but still want to change your input component class, make it a subclass
// 				// of the UFIInputComponent or modify this component accordingly.
// 				UFIInputComponent* LyraIC = Cast<UFIInputComponent>(PlayerInputComponent);
// 				if (ensureMsgf(LyraIC, TEXT("Unexpected Input Component class! The Gameplay Abilities will not be bound to their inputs. Change the input component to UFIInputComponent or a subclass of it.")))
// 				{
// 					// Add the key mappings that may have been set by the player
// 					//LyraIC->AddInputMappings(InputConfig, Subsystem);
//
// 					// This is where we actually bind and input action to a gameplay tag, which means that Gameplay Ability Blueprints will
// 					// be triggered directly by these input actions Triggered events. 
// 					TArray<uint32> BindHandles;
// 					LyraIC->BindAbilityActions(InputConfig, this, &ThisClass::Input_AbilityInputTagPressed, &ThisClass::Input_AbilityInputTagReleased, /*out*/ BindHandles);
//
// 					LyraIC->BindNativeAction(InputConfig, FIGameplayTags::InputTag_Move, ETriggerEvent::Triggered, this, &ThisClass::Input_Move, /*bLogIfNotFound=*/ false);
// 					LyraIC->BindNativeAction(InputConfig, FIGameplayTags::InputTag_Look_Mouse, ETriggerEvent::Triggered, this, &ThisClass::Input_LookMouse, /*bLogIfNotFound=*/ false);
// 					LyraIC->BindNativeAction(InputConfig, FIGameplayTags::InputTag_Look_Stick, ETriggerEvent::Triggered, this, &ThisClass::Input_LookStick, /*bLogIfNotFound=*/ false);
// 					LyraIC->BindNativeAction(InputConfig, FIGameplayTags::InputTag_Crouch, ETriggerEvent::Triggered, this, &ThisClass::Input_Crouch, /*bLogIfNotFound=*/ false);
// 					LyraIC->BindNativeAction(InputConfig, FIGameplayTags::InputTag_AutoRun, ETriggerEvent::Triggered, this, &ThisClass::Input_AutoRun, /*bLogIfNotFound=*/ false);
// 				}
// 			}
// 		}
// 	}
//
// 	if (ensure(!bReadyToBindInputs))
// 	{
// 		bReadyToBindInputs = true;
// 	}
//  
// 	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(const_cast<APlayerController*>(PC), NAME_BindInputsNow);
// 	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(const_cast<APawn*>(Pawn), NAME_BindInputsNow);
// }
//
// void UFIHeroComponent::AddAdditionalInputConfig(const UFIInputConfig* InputConfig)
// {
// 	TArray<uint32> BindHandles;
//
// 	const APawn* Pawn = GetPawn<APawn>();
// 	if (!Pawn)
// 	{
// 		return;
// 	}
// 	
// 	const APlayerController* PC = GetController<APlayerController>();
// 	check(PC);
//
// 	const ULocalPlayer* LP = PC->GetLocalPlayer();
// 	check(LP);
//
// 	UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
// 	check(Subsystem);
//
// 	if (const UFIPawnExtensionComponent* PawnExtComp = UFIPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
// 	{
// 		UFIInputComponent* LyraIC = Pawn->FindComponentByClass<UFIInputComponent>();
// 		if (ensureMsgf(LyraIC, TEXT("Unexpected Input Component class! The Gameplay Abilities will not be bound to their inputs. Change the input component to UFIInputComponent or a subclass of it.")))
// 		{
// 			LyraIC->BindAbilityActions(InputConfig, this, &ThisClass::Input_AbilityInputTagPressed, &ThisClass::Input_AbilityInputTagReleased, /*out*/ BindHandles);
// 		}
// 	}
// }
//
// void UFIHeroComponent::RemoveAdditionalInputConfig(const UFIInputConfig* InputConfig)
// {
// 	//@TODO: Implement me!
// }
//
// bool UFIHeroComponent::IsReadyToBindInputs() const
// {
// 	return bReadyToBindInputs;
// }
//
// void UFIHeroComponent::Input_AbilityInputTagPressed(FGameplayTag InputTag)
// {
// 	if (const APawn* Pawn = GetPawn<APawn>())
// 	{
// 		if (const UFIPawnExtensionComponent* PawnExtComp = UFIPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
// 		{
// 			if (UFIAbilitySystemComponent* LyraASC = PawnExtComp->GetLyraAbilitySystemComponent())
// 			{
// 				LyraASC->AbilityInputTagPressed(InputTag);
// 			}
// 		}	
// 	}
// }
//
// void UFIHeroComponent::Input_AbilityInputTagReleased(FGameplayTag InputTag)
// {
// 	const APawn* Pawn = GetPawn<APawn>();
// 	if (!Pawn)
// 	{
// 		return;
// 	}
//
// 	if (const UFIPawnExtensionComponent* PawnExtComp = UFIPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
// 	{
// 		if (UFIAbilitySystemComponent* LyraASC = PawnExtComp->GetLyraAbilitySystemComponent())
// 		{
// 			LyraASC->AbilityInputTagReleased(InputTag);
// 		}
// 	}
// }
//
// void UFIHeroComponent::Input_Move(const FInputActionValue& InputActionValue)
// {
// 	APawn* Pawn = GetPawn<APawn>();
// 	AController* Controller = Pawn ? Pawn->GetController() : nullptr;
//
// 	// If the player has attempted to move again then cancel auto running
// 	// if (ALyraPlayerController* LyraController = Cast<ALyraPlayerController>(Controller))
// 	// {
// 	// 	LyraController->SetIsAutoRunning(false);
// 	// }
// 	//
// 	if (Controller)
// 	{
// 		const FVector2D Value = InputActionValue.Get<FVector2D>();
// 		const FRotator MovementRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);
//
// 		if (Value.X != 0.0f)
// 		{
// 			const FVector MovementDirection = MovementRotation.RotateVector(FVector::RightVector);
// 			Pawn->AddMovementInput(MovementDirection, Value.X);
// 		}
//
// 		if (Value.Y != 0.0f)
// 		{
// 			const FVector MovementDirection = MovementRotation.RotateVector(FVector::ForwardVector);
// 			Pawn->AddMovementInput(MovementDirection, Value.Y);
// 		}
// 	}
// }
//
// void UFIHeroComponent::Input_LookMouse(const FInputActionValue& InputActionValue)
// {
// 	APawn* Pawn = GetPawn<APawn>();
//
// 	if (!Pawn)
// 	{
// 		return;
// 	}
// 	
// 	const FVector2D Value = InputActionValue.Get<FVector2D>();
//
// 	if (Value.X != 0.0f)
// 	{
// 		Pawn->AddControllerYawInput(Value.X);
// 	}
//
// 	if (Value.Y != 0.0f)
// 	{
// 		Pawn->AddControllerPitchInput(Value.Y);
// 	}
// }
//
// void UFIHeroComponent::Input_LookStick(const FInputActionValue& InputActionValue)
// {
// 	APawn* Pawn = GetPawn<APawn>();
//
// 	if (!Pawn)
// 	{
// 		return;
// 	}
// 	
// 	const FVector2D Value = InputActionValue.Get<FVector2D>();
//
// 	const UWorld* World = GetWorld();
// 	check(World);
//
// 	if (Value.X != 0.0f)
// 	{
// 		Pawn->AddControllerYawInput(Value.X * LyraHero::LookYawRate * World->GetDeltaSeconds());
// 	}
//
// 	if (Value.Y != 0.0f)
// 	{
// 		Pawn->AddControllerPitchInput(Value.Y * LyraHero::LookPitchRate * World->GetDeltaSeconds());
// 	}
// }
//
// void UFIHeroComponent::Input_Crouch(const FInputActionValue& InputActionValue)
// {
// 	if (AFICharacter* Character = GetPawn<AFICharacter>())
// 	{
// 		Character->ToggleCrouch();
// 	}
// }
//
// void UFIHeroComponent::Input_AutoRun(const FInputActionValue& InputActionValue)
// {
// 	if (APawn* Pawn = GetPawn<APawn>())
// 	{
// 	// 	if (ALyraPlayerController* Controller = Cast<ALyraPlayerController>(Pawn->GetController()))
// 	// 	{
// 	// 		// Toggle auto running
// 	// 		Controller->SetIsAutoRunning(!Controller->GetIsAutoRunning());
// 	// 	}	
// 	}
// }
//
// TSubclassOf<ULyraCameraMode> UFIHeroComponent::DetermineCameraMode() const
// {
// 	// if (AbilityCameraMode)
// 	// {
// 	// 	return AbilityCameraMode;
// 	// }
//
// 	const APawn* Pawn = GetPawn<APawn>();
// 	if (!Pawn)
// 	{
// 		return nullptr;
// 	}
//
// 	// if (UFIPawnExtensionComponent* PawnExtComp = UFIPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
// 	// {
// 	// 	if (const UFIPawnData* PawnData = PawnExtComp->GetPawnData<UFIPawnData>())
// 	// 	{
// 	// 		return PawnData->DefaultCameraMode;
// 	// 	}
// 	// }
//
// 	return nullptr;
// }
//
// void UFIHeroComponent::SetAbilityCameraMode(TSubclassOf<ULyraCameraMode> CameraMode, const FGameplayAbilitySpecHandle& OwningSpecHandle)
// {
// 	if (CameraMode)
// 	{
// 		AbilityCameraMode = CameraMode;
// 		AbilityCameraModeOwningSpecHandle = OwningSpecHandle;
// 	}
// }
//
// void UFIHeroComponent::ClearAbilityCameraMode(const FGameplayAbilitySpecHandle& OwningSpecHandle)
// {
// 	if (AbilityCameraModeOwningSpecHandle == OwningSpecHandle)
// 	{
// 		AbilityCameraMode = nullptr;
// 		AbilityCameraModeOwningSpecHandle = FGameplayAbilitySpecHandle();
// 	}
// }

