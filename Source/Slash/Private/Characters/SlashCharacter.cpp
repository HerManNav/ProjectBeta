// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/SlashCharacter.h"
#include "Items/Weapons/Weapon.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"

#include "GroomComponent.h"

#include "Components/AttributesComponent.h"

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

	// Place camera
	FRotator controllerRotation = GetController()->GetControlRotation();
	controllerRotation.Pitch += -20.f;

	GetController()->SetControlRotation(controllerRotation);		// Couldn't do this in the constructor because the Controller is not yet initialized there!

	// Initialize character's mapping context for key bindings
	if (APlayerController* playerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(playerController->GetLocalPlayer()))
		{
			subsystem->AddMappingContext(echoMappingContext, 0);
		}
	}
}

void ASlashCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASlashCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* enhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		enhancedInputComponent->BindAction(moveAction, ETriggerEvent::Triggered, this, &ASlashCharacter::move);
		enhancedInputComponent->BindAction(moveAction, ETriggerEvent::Completed, this, &ASlashCharacter::move_stop);
		enhancedInputComponent->BindAction(jumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);

		enhancedInputComponent->BindAction(toggleWalkAction, ETriggerEvent::Completed, this, &ASlashCharacter::toggleWalk);

		enhancedInputComponent->BindAction(lookAction, ETriggerEvent::Triggered, this, &ASlashCharacter::look);
		enhancedInputComponent->BindAction(equipAction, ETriggerEvent::Completed, this, &ASlashCharacter::equip);
		enhancedInputComponent->BindAction(attackAction, ETriggerEvent::Completed, this, &ASlashCharacter::attack);
	}
}

void ASlashCharacter::move(const FInputActionValue& value)
{	
	if (actionState == EActionState::EAS_Attacking) return;

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

void ASlashCharacter::move_stop(const FInputActionValue& value)
{
	// ToDo: intended to use for adding some sliding after releasing the forward key ('w'), while playing the jog_to_idle animation in the abp
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

		 state = ECharacterState::ECS_EquippedOneHandedWeapon;

		 updateMaxGroundSpeed();
	 }
}

void ASlashCharacter::attack()
{
	if (canAttack())
	{
		playAttackingMontage();
		actionState = EActionState::EAS_Attacking;
	}
}

bool ASlashCharacter::canAttack()
{
	return	state != ECharacterState::ECS_Unequipped &&
		actionState == EActionState::EAS_Unoccupied;
}

void ASlashCharacter::endAttack()
{
	actionState = EActionState::EAS_Unoccupied;
}

void ASlashCharacter::playAttackingMontage()
{
	TObjectPtr<UAnimInstance> animInstance = GetMesh()->GetAnimInstance();

	if (animInstance && attackMontage)
	{
		animInstance->Montage_Play(attackMontage, 1.4f);

		FName montageSection = FName();
		switch (FMath::RandRange(0, 1))
		{
		case (0):
			montageSection = FName("Attack1");
			break;
		case (1):
			montageSection = FName("Attack2");
			break;
		case (2):
			montageSection = FName("Attack3");
			break;
		}
		animInstance->Montage_JumpToSection(montageSection, attackMontage);
	}
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
		if (state == ECharacterState::ECS_Unequipped)
			GetCharacterMovement()->MaxWalkSpeed = 200.f;
		else
			GetCharacterMovement()->MaxWalkSpeed = 115.f;
	}
	else {
		if (state == ECharacterState::ECS_Unequipped)
			GetCharacterMovement()->MaxWalkSpeed = 400.f;
		else
			GetCharacterMovement()->MaxWalkSpeed = 260.f;
	}
}
