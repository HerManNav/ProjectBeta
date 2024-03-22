// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "CharacterTypes.h"

#include "SlashCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;

class UInputMappingContext;
class UInputAction;

class UGroomComponent;

class AItem;
class AWeapon;

class UAttributesComponent;

UCLASS()
class SLASH_API ASlashCharacter : public ACharacter
{
	GENERATED_BODY()

public:

	ASlashCharacter();

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	virtual void BeginPlay() override;

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
	void move_stop(const FInputActionValue& value);
	void look(const FInputActionValue& value);

	void equip();
	void attack();

	void toggleWalk();

	/*
	* State
	*/
	bool bWalking;
	void updateMaxGroundSpeed();

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<AWeapon> weapon;

	/*
	* Montage functions
	*/

	bool canAttack();
	void playAttackingMontage();

	UFUNCTION(BlueprintCallable)
	void endAttackAnim();

	UFUNCTION(BlueprintCallable)
	void setWeaponCollision(ECollisionEnabled::Type collisionEnabled);

private:

	/*
	* States
	*/

	ECharacterState state = ECharacterState::ECS_Unequipped;

	EActionState actionState = EActionState::EAS_Unoccupied;

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	TObjectPtr<UAttributesComponent> attributes;

	/*
	* Other member variables
	*/

	UPROPERTY(VisibleInstanceOnly)
	TObjectPtr<AItem> overlappingItem;

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	TObjectPtr<UAnimMontage> attackMontage;

public:

	/*
	* Getters and setters
	*/

	FORCEINLINE void setOverlappingItem(AItem* item) { overlappingItem = item; }

	FORCEINLINE ECharacterState getCharacterState() const { return state; }

	FORCEINLINE EActionState getCharacterActionState() const { return actionState; }

};
