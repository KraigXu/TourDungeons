// Copyright 2020 Dan Kestranek.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "OCCharacterBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCharacterBaseHitReactDelegate, EOCHitReactDirection, Direction);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCharacterDiedDelegate, AOCCharacterBase*, Character);

/**
* The base Character class for the game. Everything with an AbilitySystemComponent in this game will inherit from this class.
* 游戏的基本角色类。这个游戏中所有带有AbilitySystemComponent的东西都将继承这个类。
* This class should not be instantiated and instead subclassed.
* 这个类不应该被实例化，而应该被子类化。
*/
UCLASS()
class AOCCharacterBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	// 设置此字符属性的默认值
	AOCCharacterBase(const class FObjectInitializer& ObjectInitializer);

	// Set the Hit React direction in the Animation Blueprint
	// 在动画蓝图中设置Hit React方向
	UPROPERTY(BlueprintAssignable, Category = "GASDocumentation|GDCharacter")
	FCharacterBaseHitReactDelegate ShowHitReact;

	UPROPERTY(BlueprintAssignable, Category = "GASDocumentation|GDCharacter")
	FCharacterDiedDelegate OnCharacterDied;

	// Implement IAbilitySystemInterface
	// 实现IAbilitySystemInterface
	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UFUNCTION(BlueprintCallable, Category = "GASDocumentation|GDCharacter")
	virtual bool IsAlive() const;

	// Switch on AbilityID to return individual ability levels. Hardcoded to 1 for every ability in this project.
	//打开AbilityID以返回个人能力等级。硬编码为1在这个项目中的每一个能力。
	UFUNCTION(BlueprintCallable, Category = "GASDocumentation|GDCharacter")
	virtual int32 GetAbilityLevel(EOCAbilityInputID AbilityID) const;

	// Removes all CharacterAbilities. Can only be called by the Server. Removing on the Server will remove from Client too.
	// 移除所有字符。只能由服务器调用。在服务器上删除也会从客户端删除。
	virtual void RemoveCharacterAbilities();

	UFUNCTION(BlueprintCallable)
	EOCHitReactDirection GetHitReactDirection(const FVector& ImpactPoint);

	UFUNCTION(NetMulticast, Reliable, WithValidation)
	virtual void PlayHitReact(FGameplayTag HitDirection, AActor* DamageCauser);
	virtual void PlayHitReact_Implementation(FGameplayTag HitDirection, AActor* DamageCauser);
	virtual bool PlayHitReact_Validate(FGameplayTag HitDirection, AActor* DamageCauser);
	
	/**
	* 从GDAttributeSetBase获取属性
	**/
	UFUNCTION(BlueprintCallable, Category = "OracleAbility|Character|Attributes")
	int32 GetCharacterLevel() const;

	UFUNCTION(BlueprintCallable, Category = "OracleAbility|Character|Attributes")
	float GetHealth() const;

	UFUNCTION(BlueprintCallable, Category = "OracleAbility|Character|Attributes")
	float GetMaxHealth() const;

	UFUNCTION(BlueprintCallable, Category = "OracleAbility|Character|Attributes")
	float GetMana() const;

	UFUNCTION(BlueprintCallable, Category = "OracleAbility|Character|Attributes")
	float GetMaxMana() const;

	UFUNCTION(BlueprintCallable, Category = "OracleAbility|Character|Attributes")
	float GetStamina() const;

	UFUNCTION(BlueprintCallable, Category = "OracleAbility|Character|Attributes")
	float GetMaxStamina() const;
	
	// Gets the Current value of MoveSpeed
	UFUNCTION(BlueprintCallable, Category = "OracleAbility|Character|Attributes")
	float GetMoveSpeed() const;

	// Gets the Base value of MoveSpeed
	UFUNCTION(BlueprintCallable, Category = "OracleAbility|Character|Attributes")
	float GetMoveSpeedBaseValue() const;
	
	UFUNCTION(BlueprintCallable,Category="OracleAbility|Character|Attributes")
	float GetElement() const;
	
	virtual void Die();

	UFUNCTION(BlueprintCallable, Category = "GASDocumentation|GDCharacter")
	virtual void FinishDying();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Instead of TWeakObjectPtrs, you could just have UPROPERTY() hard references or no references at all and just call
	// GetAbilitySystem() and make a GetAttributeSetBase() that can read from the PlayerState or from child classes.
	// Just make sure you test if the pointer is valid before using.
	// I opted for TWeakObjectPtrs because I didn't want a shared hard reference here and I didn't want an extra function call of getting
	// the ASC/AttributeSet from the PlayerState or child classes every time I referenced them in this base class.

	//你可以只使用UPROPERTY()硬引用或不使用任何引用，而不是使用TWeakObjectPtrs
	// GetAbilitySystem()并创建一个GetAttributeSetBase()，它可以从PlayerState或子类中读取。
	//确保在使用指针之前测试指针是否有效。
	//我选择了TWeakObjectPtrs，因为我不想在这里共享硬引用，也不想要额外的get函数调用
	// ASC/AttributeSet从PlayerState或子类每次我引用他们在这个基类。
	
	TWeakObjectPtr<class UOCAbilitySystemComponent> AbilitySystemComponent;
	TWeakObjectPtr<class UOCAttributeSetBase> AttributeSetBase;

	FGameplayTag HitDirectionFrontTag;
	FGameplayTag HitDirectionBackTag;
	FGameplayTag HitDirectionRightTag;
	FGameplayTag HitDirectionLeftTag;
	FGameplayTag DeadTag;
	FGameplayTag EffectRemoveOnDeathTag;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "GASDocumentation|GDCharacter")
	FText CharacterName;

	// Death Animation
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GASDocumentation|Animation")
	UAnimMontage* DeathMontage;

	// Default abilities for this Character. These will be removed on Character death and regiven if Character respawns.
	////该角色的默认能力。这些将在角色死亡时移除，并在角色重生时给予。
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "GASDocumentation|Abilities")
	TArray<TSubclassOf<class UOCGameplayAbility>> CharacterAbilities;

	// Default attributes for a character for initializing on spawn/respawn.
	// This is an instant GE that overrides the values for attributes that get reset on spawn/respawn.
	//在刷出/重生时初始化角色的默认属性
	//这是一个即时的GE，覆盖在刷出/重生时重置的属性值。
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "GASDocumentation|Abilities")
	TSubclassOf<class UGameplayEffect> DefaultAttributes;

	// These effects are only applied one time on startup
	//这些效果只在启动时应用一次
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "GASDocumentation|Abilities")
	TArray<TSubclassOf<class UGameplayEffect>> StartupEffects;

	// Grant abilities on the Server. The Ability Specs will be replicated to the owning client.
	//在服务器上授予技能。能力规格将被复制到拥有的客户端。
	virtual void AddCharacterAbilities();

	// 初始化角色的属性。必须在服务器上运行，但我们也在客户端上运行
	// 这样我们就不用等了。服务器对客户机的复制将不再重要
	// 值应该是相同的。
	virtual void InitializeAttributes();

	virtual void AddStartupEffects();
	
	/**
	 * 属性设置。只有在特殊情况下使用这些，如重生，否则使用GE来改变属性。
	 * 这些改变属性的基本值。
	 */
	virtual void SetHealth(float Health);
	virtual void SetMana(float Mana);
	virtual void SetStamina(float Stamina);
};
