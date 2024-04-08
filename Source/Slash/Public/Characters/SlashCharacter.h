// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "InputActionValue.h"
#include "CharacterTypes.h"

#include "SlashCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;

class UInputMappingContext;
class UInputAction;

class UGroomComponent;

class AItem;
class ASlashHUD;

UCLASS()
class SLASH_API ASlashCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:

	ASlashCharacter();

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	/** <IHitInterface> */
	virtual void GetHit_Implementation(const FVector& HitPoint) override;
	/** </IHitInterface> */

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
	virtual bool CanTakeDamage() override;
	virtual void ActuallyReceiveDamage(float DamageAmount) override;
	virtual void UpdateHealthBar() override;
	/** </ABaseCharacter> */

	/** Input callbacks */
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Equip();
	void ToggleWalk();

	/** State */

	void UpdateMaxGroundSpeed();

	/*
	* Variables
	*/

	bool bWalking;

	/** Input */

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputMappingContext> EchoMappingContext;

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


private:

	/**
	* Methods 
	*/

	/** Initialization */

	void InitCameraController();
	void InitMappingContext();
	void InitHUD();

	/** 
	* Variables
	*/

	/** State */

	EEquipState EquipState = EEquipState::EES_Unequipped;
	EActionState ActionState = EActionState::EAS_Unoccupied;

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

public:

	/** Getters and setters */

	FORCEINLINE void SetOverlappingItem(AItem* Item) { OverlappingItem = Item; }

	FORCEINLINE EEquipState GetEquipState() const { return EquipState; }

	FORCEINLINE EActionState GetCharacterActionState() const { return ActionState; }

};
