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
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(GetRootComponent());
	SpringArm->TargetArmLength = 300.f;

	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
	ViewCamera->SetupAttachment(SpringArm);

	Hair = CreateDefaultSubobject<UGroomComponent>(TEXT("Hair"));
	Hair->SetupAttachment(GetMesh());
	Hair->AttachmentName = FString("head");

	Eyebrows = CreateDefaultSubobject<UGroomComponent>(TEXT("Eyebrows"));
	Eyebrows->SetupAttachment(GetMesh());
	Eyebrows->AttachmentName = FString("head");

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetGenerateOverlapEvents(true);
	GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
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
			Subsystem->AddMappingContext(EchoMappingContext, 0);
		}
	}
}

void ASlashCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Look);
		EnhancedInputComponent->BindAction(EquipAction, ETriggerEvent::Completed, this, &ASlashCharacter::Equip);
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Completed, this, &ASlashCharacter::Attack);

		EnhancedInputComponent->BindAction(ToggleWalkAction, ETriggerEvent::Completed, this, &ASlashCharacter::ToggleWalk);

		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
	}
}

float ASlashCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

void ASlashCharacter::GetHit_Implementation(const FVector& HitPoint)
{
	AttackEnd();

	ActionState = EActionState::EAS_HitReacting;
	Super::GetHit_Implementation(HitPoint);
}

void ASlashCharacter::Move(const FInputActionValue& Value)
{	
	if (ActionState == EActionState::EAS_Attacking) return;

	const FVector2D Amount = Value.Get<FVector2D>();
	if (Controller)
	{
		const FVector forwardDirection = FVector::CrossProduct(ViewCamera->GetRightVector(), FVector::UpVector);
		const FVector rightDirection = ViewCamera->GetRightVector();
		AddMovementInput(forwardDirection, Amount.X);
		AddMovementInput(rightDirection, Amount.Y);

		// This is the solution presented by Stephen (the result is the same). I like my solution (this one's maybe better in terms of performance)
		/*const FRotator ControlRotation = GetControlRotation();
		const FRotator YawRotation(0.f, ControlRotation.Yaw, 0.f);

		const FVector forwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(forwardDirection, Amount.X);

		const FVector rightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(rightDirection, Amount.Y);*/
	}
}

void ASlashCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D Amount = Value.Get<FVector2D>();
	if (Controller)
	{
		AddControllerYawInput(Amount.X);
		AddControllerPitchInput(Amount.Y);
		//viewCamera->AddLocalRotation(FRotator(Amount.Y, Amount.X, 0.f));		// This would move only the camera, not the whole bunch of components
	}
}

void ASlashCharacter::Equip()
{
	 TObjectPtr<AWeapon> WeaponToEquip = Cast<AWeapon>(OverlappingItem);
	 if (WeaponToEquip)
	 {
		 WeaponToEquip->Equip(this->GetMesh(), FName("socket_rightHand"), this, this);
		 Weapon = WeaponToEquip;

		 EquipState = EEquipState::EES_EquippedOneHandedWeapon;

		 UpdateMaxGroundSpeed();
	 }
}

bool ASlashCharacter::CanAttack()
{
	return	EquipState != EEquipState::EES_Unequipped &&
			ActionState == EActionState::EAS_Unoccupied;
}

void ASlashCharacter::Attack()
{
	Super::Attack();

	if (CanAttack())
	{
		if (PlayAttackingMontage() != -1)
			ActionState = EActionState::EAS_Attacking;
	}
}

void ASlashCharacter::AttackEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void ASlashCharacter::ToggleWalk()
{
	bWalking = !bWalking;

	UpdateMaxGroundSpeed();
}

void ASlashCharacter::UpdateMaxGroundSpeed()
{
	if (bWalking)
	{
		if (EquipState == EEquipState::EES_Unequipped)
			GetCharacterMovement()->MaxWalkSpeed = Attributes->GetWalkingSpeedUnequipped();
		else
			GetCharacterMovement()->MaxWalkSpeed = Attributes->GetWalkingSpeedEquipped();
	}
	else {
		if (EquipState == EEquipState::EES_Unequipped)
			GetCharacterMovement()->MaxWalkSpeed = Attributes->GetRunningSpeedUnequipped();
		else
			GetCharacterMovement()->MaxWalkSpeed = Attributes->GetRunningSpeedEquipped();
	}
}

void ASlashCharacter::HitReactEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}

bool ASlashCharacter::CanTakeDamage()
{
	return Super::CanTakeDamage();
}

void ASlashCharacter::ActuallyReceiveDamage(float DamageAmount)
{
	Super::ActuallyReceiveDamage(DamageAmount);
}

void ASlashCharacter::UpdateHealthBar()
{
}
