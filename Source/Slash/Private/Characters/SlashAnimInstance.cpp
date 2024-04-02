// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/SlashAnimInstance.h"
#include "Characters/SlashCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void USlashAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	SlashCharacter = Cast<ASlashCharacter>(TryGetPawnOwner());
	if (SlashCharacter)
		SlashCharacterMovement = SlashCharacter->GetCharacterMovement();
}

void USlashAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (SlashCharacterMovement)
	{
		GroundSpeed = UKismetMathLibrary::VSizeXY(SlashCharacterMovement->Velocity);	// groundSpeed = slashCharacter->GetVelocity().Length();	// This would work for our case but will get the speed in the 3 axis, not only in the xy
		bIsFalling = SlashCharacterMovement->IsFalling();
		EquipState = SlashCharacter->GetEquipState();
	}
}
