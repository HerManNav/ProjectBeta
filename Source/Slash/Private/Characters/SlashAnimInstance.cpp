// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/SlashAnimInstance.h"
#include "Characters/SlashCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void USlashAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	slashCharacter = Cast<ASlashCharacter>(TryGetPawnOwner());
	if (slashCharacter)
		slashCharacterMovement = slashCharacter->GetCharacterMovement();
}

void USlashAnimInstance::NativeUpdateAnimation(float deltaTime)
{
	Super::NativeUpdateAnimation(deltaTime);

	if (slashCharacterMovement)
	{
		groundSpeed = UKismetMathLibrary::VSizeXY(slashCharacterMovement->Velocity);	// groundSpeed = slashCharacter->GetVelocity().Length();	// This would work for our case but will get the speed in the 3 axis, not only in the xy
		bIsFalling = slashCharacterMovement->IsFalling();
		characterState = slashCharacter->getCharacterState();
	}
}
