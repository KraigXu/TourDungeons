// Copyright Epic Games, Inc. All Rights Reserved.

#include "FICharacter.h"

#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "FICharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "TimerManager.h"
#include "FateIronPro/FIGameplayTags.h"
#include "FateIronPro/FILogChannels.h"
#include "FateIronPro/AbilitySystem/FIAbilitySystemComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FICharacter)

class AActor;
class FLifetimeProperty;
class IRepChangedPropertyTracker;
class UInputComponent;

static FName NAME_LyraCharacterCollisionProfile_Capsule(TEXT("LyraPawnCapsule"));
static FName NAME_LyraCharacterCollisionProfile_Mesh(TEXT("LyraPawnMesh"));

AFICharacter::AFICharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UFICharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	// Avoid ticking characters if possible.
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	NetCullDistanceSquared = 900000000.0f;

	UCapsuleComponent* CapsuleComp = GetCapsuleComponent();
	check(CapsuleComp);
	CapsuleComp->InitCapsuleSize(40.0f, 90.0f);
	CapsuleComp->SetCollisionProfileName(NAME_LyraCharacterCollisionProfile_Capsule);

	USkeletalMeshComponent* MeshComp = GetMesh();
	check(MeshComp);
	MeshComp->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));  // Rotate mesh to be X forward since it is exported as Y forward.
	MeshComp->SetCollisionProfileName(NAME_LyraCharacterCollisionProfile_Mesh);

	UFICharacterMovementComponent* LyraMoveComp = CastChecked<UFICharacterMovementComponent>(GetCharacterMovement());
	LyraMoveComp->GravityScale = 1.0f;
	LyraMoveComp->MaxAcceleration = 2400.0f;
	LyraMoveComp->BrakingFrictionFactor = 1.0f;
	LyraMoveComp->BrakingFriction = 6.0f;
	LyraMoveComp->GroundFriction = 8.0f;
	LyraMoveComp->BrakingDecelerationWalking = 1400.0f;
	LyraMoveComp->bUseControllerDesiredRotation = false;
	LyraMoveComp->bOrientRotationToMovement = false;
	LyraMoveComp->RotationRate = FRotator(0.0f, 720.0f, 0.0f);
	LyraMoveComp->bAllowPhysicsRotationDuringAnimRootMotion = false;
	LyraMoveComp->GetNavAgentPropertiesRef().bCanCrouch = true;
	LyraMoveComp->bCanWalkOffLedgesWhenCrouching = true;
	LyraMoveComp->SetCrouchedHalfHeight(65.0f);
//
	//PawnExtComponent = CreateDefaultSubobject<UFIPawnExtensionComponent>(TEXT("PawnExtensionComponent"));
	//PawnExtComponent->OnAbilitySystemInitialized_RegisterAndCall(FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::OnAbilitySystemInitialized));
	//PawnExtComponent->OnAbilitySystemUninitialized_Register(FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::OnAbilitySystemUninitialized));
//
	//HealthComponent = CreateDefaultSubobject<UFIHealthComponent>(TEXT("HealthComponent"));
	//HealthComponent->OnDeathStarted.AddDynamic(this, &ThisClass::OnDeathStarted);
	//HealthComponent->OnDeathFinished.AddDynamic(this, &ThisClass::OnDeathFinished);
//
	//CameraComponent = CreateDefaultSubobject<UFICameraComponent>(TEXT("CameraComponent"));
	//CameraComponent->SetRelativeLocation(FVector(-300.0f, 0.0f, 75.0f));

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	BaseEyeHeight = 80.0f;
	CrouchedEyeHeight = 50.0f;
}

void AFICharacter::PreInitializeComponents()
{
	Super::PreInitializeComponents();
}

void AFICharacter::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld();

	const bool bRegisterWithSignificanceManager = !IsNetMode(NM_DedicatedServer);
	if (bRegisterWithSignificanceManager)
	{
// 		if (ULyraSignificanceManager* SignificanceManager = USignificanceManager::Get<ULyraSignificanceManager>(World))
// 		{
// //@TODO: SignificanceManager->RegisterObject(this, (EFortSignificanceType)SignificanceType);
// 		}
	}
}

void AFICharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	UWorld* World = GetWorld();

	const bool bRegisterWithSignificanceManager = !IsNetMode(NM_DedicatedServer);
	if (bRegisterWithSignificanceManager)
	{
		// if (ULyraSignificanceManager* SignificanceManager = USignificanceManager::Get<ULyraSignificanceManager>(World))
		// {
		// 	SignificanceManager->UnregisterObject(this);
		// }
	}
}

void AFICharacter::Reset()
{
	DisableMovementAndCollision();

	K2_OnReset();

	UninitAndDestroy();
}

void AFICharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ThisClass, ReplicatedAcceleration, COND_SimulatedOnly);
	//DOREPLIFETIME(ThisClass, MyTeamID)
}

void AFICharacter::PreReplication(IRepChangedPropertyTracker& ChangedPropertyTracker)
{
	Super::PreReplication(ChangedPropertyTracker);

	if (UCharacterMovementComponent* MovementComponent = GetCharacterMovement())
	{
		// Compress Acceleration: XY components as direction + magnitude, Z component as direct value
		const double MaxAccel = MovementComponent->MaxAcceleration;
		const FVector CurrentAccel = MovementComponent->GetCurrentAcceleration();
		double AccelXYRadians, AccelXYMagnitude;
		FMath::CartesianToPolar(CurrentAccel.X, CurrentAccel.Y, AccelXYMagnitude, AccelXYRadians);

		ReplicatedAcceleration.AccelXYRadians   = FMath::FloorToInt((AccelXYRadians / TWO_PI) * 255.0);     // [0, 2PI] -> [0, 255]
		ReplicatedAcceleration.AccelXYMagnitude = FMath::FloorToInt((AccelXYMagnitude / MaxAccel) * 255.0);	// [0, MaxAccel] -> [0, 255]
		ReplicatedAcceleration.AccelZ           = FMath::FloorToInt((CurrentAccel.Z / MaxAccel) * 127.0);   // [-MaxAccel, MaxAccel] -> [-127, 127]
	}
}

void AFICharacter::NotifyControllerChanged()
{
	// const FGenericTeamId OldTeamId = GetGenericTeamId();
	//
	// Super::NotifyControllerChanged();
	//
	// // Update our team ID based on the controller
	// if (HasAuthority() && (Controller != nullptr))
	// {
	// 	if (ILyraTeamAgentInterface* ControllerWithTeam = Cast<ILyraTeamAgentInterface>(Controller))
	// 	{
	// 		MyTeamID = ControllerWithTeam->GetGenericTeamId();
	// 		ConditionalBroadcastTeamChanged(this, OldTeamId, MyTeamID);
	// 	}
	// }
}

// ALyraPlayerController* AFICharacter::GetLyraPlayerController() const
// {
// 	return CastChecked<ALyraPlayerController>(Controller, ECastCheckedType::NullAllowed);
// }
//
// ALyraPlayerState* AFICharacter::GetLyraPlayerState() const
// {
// 	return CastChecked<ALyraPlayerState>(GetPlayerState(), ECastCheckedType::NullAllowed);
// }
//
UFIAbilitySystemComponent* AFICharacter::GetFIAbilitySystemComponent() const
{
	return Cast<UFIAbilitySystemComponent>(GetAbilitySystemComponent());
}

UAbilitySystemComponent* AFICharacter::GetAbilitySystemComponent() const
{
	// if (PawnExtComponent == nullptr)
	// {
	// 	return nullptr;
	// }

	//return PawnExtComponent->GetLyraAbilitySystemComponent();
	return nullptr;
}

void AFICharacter::OnAbilitySystemInitialized()
{
	// UFIAbilitySystemComponent* LyraASC = GetLyraAbilitySystemComponent();
	// check(LyraASC);
	//
	// HealthComponent->InitializeWithAbilitySystem(LyraASC);
	//
	// InitializeGameplayTags();
}

void AFICharacter::OnAbilitySystemUninitialized()
{
	//HealthComponent->UninitializeFromAbilitySystem();
}

void AFICharacter::PossessedBy(AController* NewController)
{
	// const FGenericTeamId OldTeamID = MyTeamID;
	//
	// Super::PossessedBy(NewController);
	//
	// PawnExtComponent->HandleControllerChanged();
	//
	// // Grab the current team ID and listen for future changes
	// if (ILyraTeamAgentInterface* ControllerAsTeamProvider = Cast<ILyraTeamAgentInterface>(NewController))
	// {
	// 	MyTeamID = ControllerAsTeamProvider->GetGenericTeamId();
	// 	ControllerAsTeamProvider->GetTeamChangedDelegateChecked().AddDynamic(this, &ThisClass::OnControllerChangedTeam);
	// }
	// ConditionalBroadcastTeamChanged(this, OldTeamID, MyTeamID);
}

void AFICharacter::UnPossessed()
{
	// AController* const OldController = Controller;
	//
	// // Stop listening for changes from the old controller
	// const FGenericTeamId OldTeamID = MyTeamID;
	// if (ILyraTeamAgentInterface* ControllerAsTeamProvider = Cast<ILyraTeamAgentInterface>(OldController))
	// {
	// 	ControllerAsTeamProvider->GetTeamChangedDelegateChecked().RemoveAll(this);
	// }
	//
	// Super::UnPossessed();
	//
	// PawnExtComponent->HandleControllerChanged();
	//
	// // Determine what the new team ID should be afterwards
	// MyTeamID = DetermineNewTeamAfterPossessionEnds(OldTeamID);
	// ConditionalBroadcastTeamChanged(this, OldTeamID, MyTeamID);
}

void AFICharacter::OnRep_Controller()
{
	Super::OnRep_Controller();

	//PawnExtComponent->HandleControllerChanged();
}

void AFICharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	//PawnExtComponent->HandlePlayerStateReplicated();
}

void AFICharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//PawnExtComponent->SetupPlayerInputComponent();
}

void AFICharacter::InitializeGameplayTags()
{
	// Clear tags that may be lingering on the ability system from the previous pawn.
	if (UFIAbilitySystemComponent* LyraASC = GetFIAbilitySystemComponent())
	{
		for (const TPair<uint8, FGameplayTag>& TagMapping : FIGameplayTags::MovementModeTagMap)
		{
			if (TagMapping.Value.IsValid())
			{
				LyraASC->SetLooseGameplayTagCount(TagMapping.Value, 0);
			}
		}

		for (const TPair<uint8, FGameplayTag>& TagMapping : FIGameplayTags::CustomMovementModeTagMap)
		{
			if (TagMapping.Value.IsValid())
			{
				LyraASC->SetLooseGameplayTagCount(TagMapping.Value, 0);
			}
		}

		UFICharacterMovementComponent* LyraMoveComp = CastChecked<UFICharacterMovementComponent>(GetCharacterMovement());
		SetMovementModeTag(LyraMoveComp->MovementMode, LyraMoveComp->CustomMovementMode, true);
	}
}

void AFICharacter::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	if (const UFIAbilitySystemComponent* LyraASC = GetFIAbilitySystemComponent())
	{
		LyraASC->GetOwnedGameplayTags(TagContainer);
	}
}

bool AFICharacter::HasMatchingGameplayTag(FGameplayTag TagToCheck) const
{
	if (const UFIAbilitySystemComponent* LyraASC = GetFIAbilitySystemComponent())
	{
		return LyraASC->HasMatchingGameplayTag(TagToCheck);
	}

	return false;
}

bool AFICharacter::HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	if (const UFIAbilitySystemComponent* LyraASC = GetFIAbilitySystemComponent())
	{
		return LyraASC->HasAllMatchingGameplayTags(TagContainer);
	}

	return false;
}

bool AFICharacter::HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	if (const UFIAbilitySystemComponent* LyraASC = GetFIAbilitySystemComponent())
	{
		return LyraASC->HasAnyMatchingGameplayTags(TagContainer);
	}

	return false;
}

void AFICharacter::FellOutOfWorld(const class UDamageType& dmgType)
{
	//HealthComponent->DamageSelfDestruct(/*bFellOutOfWorld=*/ true);
}

void AFICharacter::OnDeathStarted(AActor*)
{
	DisableMovementAndCollision();
}

void AFICharacter::OnDeathFinished(AActor*)
{
	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::DestroyDueToDeath);
}


void AFICharacter::DisableMovementAndCollision()
{
	if (Controller)
	{
		Controller->SetIgnoreMoveInput(true);
	}

	UCapsuleComponent* CapsuleComp = GetCapsuleComponent();
	check(CapsuleComp);
	CapsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CapsuleComp->SetCollisionResponseToAllChannels(ECR_Ignore);

	UFICharacterMovementComponent* LyraMoveComp = CastChecked<UFICharacterMovementComponent>(GetCharacterMovement());
	LyraMoveComp->StopMovementImmediately();
	LyraMoveComp->DisableMovement();
}

void AFICharacter::DestroyDueToDeath()
{
	K2_OnDeathFinished();

	UninitAndDestroy();
}


void AFICharacter::UninitAndDestroy()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		DetachFromControllerPendingDestroy();
		SetLifeSpan(0.1f);
	}

	// Uninitialize the ASC if we're still the avatar actor (otherwise another pawn already did it when they became the avatar actor)
	if (UFIAbilitySystemComponent* LyraASC = GetFIAbilitySystemComponent())
	{
		if (LyraASC->GetAvatarActor() == this)
		{
			//PawnExtComponent->UninitializeAbilitySystem();
		}
	}

	SetActorHiddenInGame(true);
}

void AFICharacter::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);

	UFICharacterMovementComponent* LyraMoveComp = CastChecked<UFICharacterMovementComponent>(GetCharacterMovement());

	SetMovementModeTag(PrevMovementMode, PreviousCustomMode, false);
	SetMovementModeTag(LyraMoveComp->MovementMode, LyraMoveComp->CustomMovementMode, true);
}

void AFICharacter::SetMovementModeTag(EMovementMode MovementMode, uint8 CustomMovementMode, bool bTagEnabled)
{
	if (UFIAbilitySystemComponent* LyraASC = GetFIAbilitySystemComponent())
	{
		const FGameplayTag* MovementModeTag = nullptr;
		if (MovementMode == MOVE_Custom)
		{
			MovementModeTag = FIGameplayTags::CustomMovementModeTagMap.Find(CustomMovementMode);
		}
		else
		{
			MovementModeTag = FIGameplayTags::MovementModeTagMap.Find(MovementMode);
		}

		if (MovementModeTag && MovementModeTag->IsValid())
		{
			LyraASC->SetLooseGameplayTagCount(*MovementModeTag, (bTagEnabled ? 1 : 0));
		}
	}
}

void AFICharacter::ToggleCrouch()
{
	const UFICharacterMovementComponent* LyraMoveComp = CastChecked<UFICharacterMovementComponent>(GetCharacterMovement());

	if (bIsCrouched || LyraMoveComp->bWantsToCrouch)
	{
		UnCrouch();
	}
	else if (LyraMoveComp->IsMovingOnGround())
	{
		Crouch();
	}
}

void AFICharacter::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	if (UFIAbilitySystemComponent* LyraASC = GetFIAbilitySystemComponent())
	{
		LyraASC->SetLooseGameplayTagCount(FIGameplayTags::Status_Crouching, 1);
	}


	Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
}

void AFICharacter::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	if (UFIAbilitySystemComponent* LyraASC = GetFIAbilitySystemComponent())
	{
		LyraASC->SetLooseGameplayTagCount(FIGameplayTags::Status_Crouching, 0);
	}

	Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
}

bool AFICharacter::CanJumpInternal_Implementation() const
{
	// same as ACharacter's implementation but without the crouch check
	return JumpIsAllowedInternal();
}

void AFICharacter::OnRep_ReplicatedAcceleration()
{
	if (UFICharacterMovementComponent* LyraMovementComponent = Cast<UFICharacterMovementComponent>(GetCharacterMovement()))
	{
		// Decompress Acceleration
		const double MaxAccel         = LyraMovementComponent->MaxAcceleration;
		const double AccelXYMagnitude = double(ReplicatedAcceleration.AccelXYMagnitude) * MaxAccel / 255.0; // [0, 255] -> [0, MaxAccel]
		const double AccelXYRadians   = double(ReplicatedAcceleration.AccelXYRadians) * TWO_PI / 255.0;     // [0, 255] -> [0, 2PI]

		FVector UnpackedAcceleration(FVector::ZeroVector);
		FMath::PolarToCartesian(AccelXYMagnitude, AccelXYRadians, UnpackedAcceleration.X, UnpackedAcceleration.Y);
		UnpackedAcceleration.Z = double(ReplicatedAcceleration.AccelZ) * MaxAccel / 127.0; // [-127, 127] -> [-MaxAccel, MaxAccel]

		LyraMovementComponent->SetReplicatedAcceleration(UnpackedAcceleration);
	}
}
//
// void AFICharacter::SetGenericTeamId(const FGenericTeamId& NewTeamID)
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
// 			UE_LOG(LogFIPawn, Error, TEXT("You can't set the team ID on a character (%s) except on the authority"), *GetPathNameSafe(this));
// 		}
// 	}
// 	else
// 	{
// 		UE_LOG(LogFIPawn, Error, TEXT("You can't set the team ID on a possessed character (%s); it's driven by the associated controller"), *GetPathNameSafe(this));
// 	}
// }
//
// FGenericTeamId AFICharacter::GetGenericTeamId() const
// {
// 	return MyTeamID;
// }
//
// FOnLyraTeamIndexChangedDelegate* AFICharacter::GetOnTeamIndexChangedDelegate()
// {
// 	return &OnTeamChangedDelegate;
// }
//
// void AFICharacter::OnControllerChangedTeam(UObject* TeamAgent, int32 OldTeam, int32 NewTeam)
// {
// 	const FGenericTeamId MyOldTeamID = MyTeamID;
// 	MyTeamID = IntegerToGenericTeamId(NewTeam);
// 	ConditionalBroadcastTeamChanged(this, MyOldTeamID, MyTeamID);
// }
//
// void AFICharacter::OnRep_MyTeamID(FGenericTeamId OldTeamID)
// {
// 	ConditionalBroadcastTeamChanged(this, OldTeamID, MyTeamID);
// }

bool AFICharacter::UpdateSharedReplication()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		FSharedRepMovement SharedMovement;
		if (SharedMovement.FillForCharacter(this))
		{
			// Only call FastSharedReplication if data has changed since the last frame.
			// Skipping this call will cause replication to reuse the same bunch that we previously
			// produced, but not send it to clients that already received. (But a new client who has not received
			// it, will get it this frame)
			if (!SharedMovement.Equals(LastSharedReplication, this))
			{
				LastSharedReplication = SharedMovement;
				ReplicatedMovementMode = SharedMovement.RepMovementMode;

				FastSharedReplication(SharedMovement);
			}
			return true;
		}
	}

	// We cannot fastrep right now. Don't send anything.
	return false;
}

void AFICharacter::FastSharedReplication_Implementation(const FSharedRepMovement& SharedRepMovement)
{
	if (GetWorld()->IsPlayingReplay())
	{
		return;
	}

	// Timestamp is checked to reject old moves.
	if (GetLocalRole() == ROLE_SimulatedProxy)
	{
		// Timestamp
		ReplicatedServerLastTransformUpdateTimeStamp = SharedRepMovement.RepTimeStamp;

		// Movement mode
		if (ReplicatedMovementMode != SharedRepMovement.RepMovementMode)
		{
			ReplicatedMovementMode = SharedRepMovement.RepMovementMode;
			GetCharacterMovement()->bNetworkMovementModeChanged = true;
			GetCharacterMovement()->bNetworkUpdateReceived = true;
		}

		// Location, Rotation, Velocity, etc.
		FRepMovement& MutableRepMovement = GetReplicatedMovement_Mutable();
		MutableRepMovement = SharedRepMovement.RepMovement;

		// This also sets LastRepMovement
		OnRep_ReplicatedMovement();

		// Jump force
		bProxyIsJumpForceApplied = SharedRepMovement.bProxyIsJumpForceApplied;

		// Crouch
		if (bIsCrouched != SharedRepMovement.bIsCrouched)
		{
			bIsCrouched = SharedRepMovement.bIsCrouched;
			OnRep_IsCrouched();
		}
	}
}

FSharedRepMovement::FSharedRepMovement()
{
	RepMovement.LocationQuantizationLevel = EVectorQuantization::RoundTwoDecimals;
}

bool FSharedRepMovement::FillForCharacter(ACharacter* Character)
{
	if (USceneComponent* PawnRootComponent = Character->GetRootComponent())
	{
		UCharacterMovementComponent* CharacterMovement = Character->GetCharacterMovement();

		RepMovement.Location = FRepMovement::RebaseOntoZeroOrigin(PawnRootComponent->GetComponentLocation(), Character);
		RepMovement.Rotation = PawnRootComponent->GetComponentRotation();
		RepMovement.LinearVelocity = CharacterMovement->Velocity;
		RepMovementMode = CharacterMovement->PackNetworkMovementMode();
		bProxyIsJumpForceApplied = Character->bProxyIsJumpForceApplied || (Character->JumpForceTimeRemaining > 0.0f);
		bIsCrouched = Character->bIsCrouched;

		// Timestamp is sent as zero if unused
		if ((CharacterMovement->NetworkSmoothingMode == ENetworkSmoothingMode::Linear) || CharacterMovement->bNetworkAlwaysReplicateTransformUpdateTimestamp)
		{
			RepTimeStamp = CharacterMovement->GetServerLastTransformUpdateTimeStamp();
		}
		else
		{
			RepTimeStamp = 0.f;
		}

		return true;
	}
	return false;
}

bool FSharedRepMovement::Equals(const FSharedRepMovement& Other, ACharacter* Character) const
{
	if (RepMovement.Location != Other.RepMovement.Location)
	{
		return false;
	}

	if (RepMovement.Rotation != Other.RepMovement.Rotation)
	{
		return false;
	}

	if (RepMovement.LinearVelocity != Other.RepMovement.LinearVelocity)
	{
		return false;
	}

	if (RepMovementMode != Other.RepMovementMode)
	{
		return false;
	}

	if (bProxyIsJumpForceApplied != Other.bProxyIsJumpForceApplied)
	{
		return false;
	}

	if (bIsCrouched != Other.bIsCrouched)
	{
		return false;
	}

	return true;
}

bool FSharedRepMovement::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	bOutSuccess = true;
	RepMovement.NetSerialize(Ar, Map, bOutSuccess);
	Ar << RepMovementMode;
	Ar << bProxyIsJumpForceApplied;
	Ar << bIsCrouched;

	// Timestamp, if non-zero.
	uint8 bHasTimeStamp = (RepTimeStamp != 0.f);
	Ar.SerializeBits(&bHasTimeStamp, 1);
	if (bHasTimeStamp)
	{
		Ar << RepTimeStamp;
	}
	else
	{
		RepTimeStamp = 0.f;
	}

	return true;
}
