// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "InputActionValue.h"
#include "Interfaces/PickupInterface.h"

#include "SlashCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;

class UInputMappingContext;
class UInputAction;
class UEnhancedInputLocalPlayerSubsystem;

class UGroomComponent;

class AItem;
class ASlashHUD;

UCLASS()
class SLASH_API ASlashCharacter : public ABaseCharacter, public IPickupInterface
{
	GENERATED_BODY()

public:

	ASlashCharacter();

	virtual void Tick(float DeltaTime) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** <IHitInterface> */
	virtual void GetHit_Implementation(const FVector& HitPoint) override;
	/** </IHitInterface> */

	/** <IPickupInterface */
	virtual void SetOverlappingItem_Implementation(AItem* Item) override;
	virtual void PickupTreasure_Implementation(AItem* Item) override;
	virtual void PickupSoul_Implementation(AItem* Item) override;
	/** </IPickupInterface */

protected:

	virtual void BeginPlay() override;

	/**
	* Methods
	*/

	/** <ABaseCharacter> */
	virtual bool CanAttack() override;
	virtual void Attack() override;
	virtual void AttackEnd() override;
	virtual void HitReactEnd() override;
	virtual void Die() override;
	virtual void DisableCollisionsToDie() override;
	virtual int16 PlayDeathMontage() override;
	virtual bool CanTakeDamage() override;
	virtual void ActuallyReceiveDamage(float DamageAmount) override;
	virtual void UpdateHealthBar() override;
	/** </ABaseCharacter> */

	/** Input callbacks */
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Equip();
	void ToggleWalk();
	void DodgeForward();

	/** State */

	void UpdateMaxGroundSpeed();
	void UpdateStaminaBar();

	bool IsUnoccupied();
	bool IsHitReacting();
	bool IsDodging();
	bool IsAttacking();
	bool IsDead();

	bool IsUnequipped();
	bool IsEquipped();

	bool CanDodge();
	bool HasEnoughStaminaToDodge();
	void ConsumeStamina();
	void RecoverStamina(float RecoverAmount);

	/** Exposed */

	UFUNCTION(BlueprintCallable)
	void DodgeEnd();

	/*
	* Variables
	*/

	/** State */

	bool bWalking;

	EEquipState EquipState = EEquipState::EES_Unequipped;

	UPROPERTY(BlueprintReadOnly)
	EActionState ActionState = EActionState::EAS_Unoccupied;

	/** Input */

	TObjectPtr<UEnhancedInputLocalPlayerSubsystem> EnhancedInputSubsystem;

	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<UInputMappingContext> ActionsMappingContext;

	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<UInputMappingContext> LookMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputAction> ToggleWalkAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputAction> EquipAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputAction> AttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputAction> DodgeForwardAction;

	/** Dodge */

	UPROPERTY(EditAnywhere, Category = "Dodge|Stamina")
	float DodgeStaminaConsumption = 20.f;


private:

	/**
	* Methods 
	*/

	/** Initialization */

	void InitCameraController();
	void InitMappingContext();
	void InitHUD();

	/* General common methods*/
	void DisableAllInput();

	/** 
	* Variables
	*/

	/** State */

	UPROPERTY(VisibleInstanceOnly)
	TObjectPtr<AItem> OverlappingItem;

	UPROPERTY()
	TObjectPtr<ASlashHUD> HUD;

	/** Components */

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpringArmComponent> SpringArm;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCameraComponent> ViewCamera;

	UPROPERTY(VisibleAnywhere, Category = Hair)
	TObjectPtr<UGroomComponent> Hair;

	UPROPERTY(VisibleAnywhere, Category = Hair)
	TObjectPtr<UGroomComponent> Eyebrows;

	/** Montages */

	UPROPERTY(EditAnywhere, Category = "Montages|Movement")
	TObjectPtr<UAnimMontage> DodgeForwardMontage;

public:

	/** Getters and setters */

	FORCEINLINE void SetOverlappingItem(AItem* Item) { OverlappingItem = Item; }

	FORCEINLINE EEquipState GetEquipState() const { return EquipState; }

	FORCEINLINE EActionState GetCharacterActionState() const { return ActionState; }

};
