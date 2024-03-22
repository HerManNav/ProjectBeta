// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CharacterTypes.h"

#include "SlashAnimInstance.generated.h"

class ASlashCharacter;
class UCharacterMovementComponent;

/**
 * 
 */
UCLASS()
class SLASH_API USlashAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:

	void NativeInitializeAnimation() override;

	void NativeUpdateAnimation(float deltaTime) override;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<ASlashCharacter> slashCharacter;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	TObjectPtr<UCharacterMovementComponent> slashCharacterMovement;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	float groundSpeed;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	bool bIsFalling;

	UPROPERTY(BlueprintReadOnly, Category = "Movement | Character state")
	ECharacterState characterState;

};
