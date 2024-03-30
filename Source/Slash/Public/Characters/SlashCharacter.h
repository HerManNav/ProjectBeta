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

UCLASS()
class SLASH_API ASlashCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:

	ASlashCharacter();

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:

	virtual void BeginPlay() override;

	/**
	* Methods
	*/

	/** <ABaseCharacter> */
	virtual bool CanAttack() override;
	virtual void Attack() override;
	virtual void AttackEnd() override;
	/** </ABaseCharacter> */

	/** Input callbacks */
	void move(const FInputActionValue& value);
	void look(const FInputActionValue& value);
	void equip();
	void toggleWalk();

	/** State */

	void updateMaxGroundSpeed();
	
	/*
	* Variables
	*/

	bool bWalking;

	/** Input */

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputMappingContext> echoMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputAction> moveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputAction> lookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputAction> jumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputAction> toggleWalkAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputAction> equipAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputAction> attackAction;


private:

	/**
	* Methods 
	*/

	/** Initialization */

	void InitCameraController();
	void InitMappingContext();

	/** 
	* Variables
	*/

	/** State */

	ECharacterState CharacterState = ECharacterState::ECS_Unequipped;
	EActionState ActionState = EActionState::EAS_Unoccupied;

	UPROPERTY(VisibleInstanceOnly)
	TObjectPtr<AItem> overlappingItem;

	/** Components */

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpringArmComponent> springArm;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCameraComponent> viewCamera;

	UPROPERTY(VisibleAnywhere, Category = Hair)
	TObjectPtr<UGroomComponent> hair;

	UPROPERTY(VisibleAnywhere, Category = Hair)
	TObjectPtr<UGroomComponent> eyebrows;

public:

	/** Getters and setters */

	FORCEINLINE void setOverlappingItem(AItem* item) { overlappingItem = item; }

	FORCEINLINE ECharacterState getCharacterState() const { return CharacterState; }

	FORCEINLINE EActionState getCharacterActionState() const { return ActionState; }

};
