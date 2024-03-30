// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/SlashCharacter.h"
#include "Items/Weapons/Weapon.h"
#include "Components/AttributesComponent.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GroomComponent.h"

#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"

ASlashCharacter::ASlashCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	bWalking = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);

	// Components
	springArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("springArm"));
	springArm->SetupAttachment(GetRootComponent());
	springArm->TargetArmLength = 300.f;

	viewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("viewCamera"));
	viewCamera->SetupAttachment(springArm);

	hair = CreateDefaultSubobject<UGroomComponent>(TEXT("hair"));
	hair->SetupAttachment(GetMesh());
	hair->AttachmentName = FString("head");

	eyebrows = CreateDefaultSubobject<UGroomComponent>(TEXT("eyebrows"));
	eyebrows->SetupAttachment(GetMesh());
	eyebrows->AttachmentName = FString("head");
}

void ASlashCharacter::BeginPlay()
{
	Super::BeginPlay();

	Tags.Add(FName("SlashCharacter"));

	InitCameraController();
	InitMappingContext();
}

void ASlashCharacter::InitCameraController()
{
	FRotator ControllerRotation = GetController()->GetControlRotation();
	ControllerRotation.Pitch += -20.f;

	GetController()->SetControlRotation(ControllerRotation);		// Couldn't do this in the constructor because the Controller is not yet initialized there!
}

void ASlashCharacter::InitMappingContext()
{
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(echoMappingContext, 0);
		}
	}
}

void ASlashCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* enhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		enhancedInputComponent->BindAction(moveAction, ETriggerEvent::Triggered, this, &ASlashCharacter::move);
		enhancedInputComponent->BindAction(lookAction, ETriggerEvent::Triggered, this, &ASlashCharacter::look);
		enhancedInputComponent->BindAction(equipAction, ETriggerEvent::Completed, this, &ASlashCharacter::equip);
		enhancedInputComponent->BindAction(attackAction, ETriggerEvent::Completed, this, &ASlashCharacter::Attack);

		enhancedInputComponent->BindAction(toggleWalkAction, ETriggerEvent::Completed, this, &ASlashCharacter::toggleWalk);

		enhancedInputComponent->BindAction(jumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
	}
}

void ASlashCharacter::move(const FInputActionValue& value)
{	
	if (ActionState == EActionState::EAS_Attacking) return;

	const FVector2D amount = value.Get<FVector2D>();
	if (Controller)
	{
		const FVector forwardDirection = FVector::CrossProduct(viewCamera->GetRightVector(), FVector::UpVector);
		const FVector rightDirection = viewCamera->GetRightVector();
		AddMovementInput(forwardDirection, amount.X);
		AddMovementInput(rightDirection, amount.Y);

		// This is the solution presented by Stephen (the result is the same). I like my solution (this one's maybe better in terms of performance)
		/*const FRotator ControlRotation = GetControlRotation();
		const FRotator YawRotation(0.f, ControlRotation.Yaw, 0.f);

		const FVector forwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(forwardDirection, amount.X);

		const FVector rightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(rightDirection, amount.Y);*/
	}
}

void ASlashCharacter::look(const FInputActionValue& value)
{
	const FVector2D amount = value.Get<FVector2D>();
	if (Controller)
	{
		AddControllerYawInput(amount.X);
		AddControllerPitchInput(amount.Y);
		//viewCamera->AddLocalRotation(FRotator(amount.Y, amount.X, 0.f));		// This would move only the camera, not the whole bunch of components
	}
}

void ASlashCharacter::equip()
{
	 TObjectPtr<AWeapon> weaponToEquip = Cast<AWeapon>(overlappingItem);
	 if (weaponToEquip)
	 {
		 weaponToEquip->Equip(this->GetMesh(), FName("socket_rightHand"), this, this);
		 weapon = weaponToEquip;

		 CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;

		 updateMaxGroundSpeed();
	 }
}

bool ASlashCharacter::CanAttack()
{
	return	CharacterState != ECharacterState::ECS_Unequipped &&
			ActionState == EActionState::EAS_Unoccupied;
}

void ASlashCharacter::Attack()
{
	Super::Attack();

	if (CanAttack())
	{
		PlayAttackingMontage();
		ActionState = EActionState::EAS_Attacking;
	}
}

void ASlashCharacter::AttackEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void ASlashCharacter::toggleWalk()
{
	bWalking = !bWalking;

	updateMaxGroundSpeed();
}

void ASlashCharacter::updateMaxGroundSpeed()
{
	if (bWalking)
	{
		if (CharacterState == ECharacterState::ECS_Unequipped)
			GetCharacterMovement()->MaxWalkSpeed = attributes->GetWalkingSpeedUnequipped();
		else
			GetCharacterMovement()->MaxWalkSpeed = attributes->GetWalkingSpeedEquipped();
	}
	else {
		if (CharacterState == ECharacterState::ECS_Unequipped)
			GetCharacterMovement()->MaxWalkSpeed = attributes->GetRunningSpeedUnequipped();
		else
			GetCharacterMovement()->MaxWalkSpeed = attributes->GetRunningSpeedEquipped();
	}
}
