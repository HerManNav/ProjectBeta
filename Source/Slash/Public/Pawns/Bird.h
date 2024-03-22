// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "InputActionValue.h"

#include "Bird.generated.h"

class UCapsuleComponent;
class USkeletalMeshComponent;
class USpringArmComponent;
class UCameraComponent;

class UInputMappingContext;
class UInputAction;

UCLASS()
class SLASH_API ABird : public APawn
{
	GENERATED_BODY()

public:

	ABird();

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:

	virtual void BeginPlay() override;

protected:

	// OLD SYSTEM: axis and actions mappings
	void moveForward(float value);
	void lookRightAndLeft(float value);
	void lookUpAndDown(float value);

	// NEW SYSTEM: enhanced input
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputMappingContext* birdMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* moveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* lookAction;

	void move(const FInputActionValue& value);
	void look(const FInputActionValue& value);

private:

	UPROPERTY(VisibleAnywhere)
	UCapsuleComponent* capsule;

	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* mesh;

	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* springArm;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* viewCamera;
};
