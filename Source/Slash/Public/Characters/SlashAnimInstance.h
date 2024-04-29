// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CharacterTypes.h"

#include "SlashAnimInstance.generated.h"

class ASlashCharacter;
class UCharacterMovementComponent;
class ULockOnComponent;

/**
 * 
 */
UCLASS()
class SLASH_API USlashAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:

	void NativeInitializeAnimation() override;

	void NativeUpdateAnimation(float DeltaTime) override;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<ASlashCharacter> SlashCharacter;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	TObjectPtr<UCharacterMovementComponent> SlashCharacterMovement;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	TObjectPtr<ULockOnComponent> SlashLockOnComponent;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	float GroundSpeed;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	float MovementDirection;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	bool bIsFalling;

	UPROPERTY(BlueprintReadOnly, Category = "Movement|Equip state")
	EEquipState EquipState;

};
