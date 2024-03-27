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

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	virtual void BeginPlay() override;

	/*
	* Input
	*/

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

	/*
	* Callbacks for input
	*/
	void move(const FInputActionValue& value);
	void moveEnd(const FInputActionValue& value);
	void look(const FInputActionValue& value);

	void equip();

	void toggleWalk();

	/*
	* State
	*/
	bool bWalking;
	void updateMaxGroundSpeed();

	/*
	* Attack
	*/

	virtual bool canAttack() override;

private:

	/*
	* State
	*/

	ECharacterState state = ECharacterState::ECS_Unequipped;

	UPROPERTY(VisibleInstanceOnly)
	TObjectPtr<AItem> overlappingItem;

	/*
	* Components
	*/

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpringArmComponent> springArm;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCameraComponent> viewCamera;

	UPROPERTY(VisibleAnywhere, Category = Hair)
	TObjectPtr<UGroomComponent> hair;

	UPROPERTY(VisibleAnywhere, Category = Hair)
	TObjectPtr<UGroomComponent> eyebrows;

public:

	/*
	* Getters and setters
	*/

	FORCEINLINE void setOverlappingItem(AItem* item) { overlappingItem = item; }

	FORCEINLINE ECharacterState getCharacterState() const { return state; }

	FORCEINLINE EActionState getCharacterActionState() const { return actionState; }

};
