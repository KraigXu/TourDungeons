// Copyright 2020 Dan Kestranek.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "OCCharacterMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class UOCCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
	
	class FGDSavedMove : public FSavedMove_Character
	{
	public:

		typedef FSavedMove_Character Super;

		///@brief 重置所有保存的变量。
		virtual void Clear() override;

		///@brief 在压缩标志中存储输入命令。
		virtual uint8 GetCompressedFlags() const override;

		///@brief用于检查两个动作是否可以合并为一个。
		///基本上你只是检查，以确保保存的变量是相同的。
		virtual bool CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* Character, float MaxDelta) const override;

		///@brief 在将移动发送到服务器之前对其进行设置。
		virtual void SetMoveFor(ACharacter* Character, float InDeltaTime, FVector const& NewAccel, class FNetworkPredictionData_Client_Character & ClientData) override;
		///@brief 在进行预测修正之前设置角色移动组件的变量。
		virtual void PrepMoveFor(class ACharacter* Character) override;

		// Sprint
		uint8 SavedRequestToStartSprinting : 1;

		// 向下瞄准
		uint8 SavedRequestToStartADS : 1;
	};

	class FGDNetworkPredictionData_Client : public FNetworkPredictionData_Client_Character
	{
	public:
		FGDNetworkPredictionData_Client(const UCharacterMovementComponent& ClientMovement);

		typedef FNetworkPredictionData_Client_Character Super;

		///@brief Allocates a new copy of our custom saved move
		virtual FSavedMovePtr AllocateNewMove() override;
	};

public:
	UOCCharacterMovementComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sprint")
	float SprintSpeedMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aim Down Sights")
	float ADSSpeedMultiplier;

	uint8 RequestToStartSprinting : 1;
	uint8 RequestToStartADS : 1;

	virtual float GetMaxSpeed() const override;
	virtual void UpdateFromCompressedFlags(uint8 Flags) override;
	virtual class FNetworkPredictionData_Client* GetPredictionData_Client() const override;

	// Sprint
	UFUNCTION(BlueprintCallable, Category = "Sprint")
	void StartSprinting();
	UFUNCTION(BlueprintCallable, Category = "Sprint")
	void StopSprinting();

	// 向下瞄准
	UFUNCTION(BlueprintCallable, Category = "Aim Down Sights")
	void StartAimDownSights();
	UFUNCTION(BlueprintCallable, Category = "Aim Down Sights")
	void StopAimDownSights();
};
