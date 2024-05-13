// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/SlashCharacter.h"
#include "Items/Weapons/Weapon.h"
#include "Items/Pickups/Treasure.h"
#include "Items/Pickups/Soul.h"
#include "Components/AttributesComponent.h"
#include "HUD/SlashHUD.h"
#include "HUD/SlashOverlay.h"
#include "Components/LockOnComponent.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GroomComponent.h"
#include "Kismet/KismetMathLibrary.h"

#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"

ASlashCharacter::ASlashCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, DefaultRotationRate, 0.f);

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

	LockOnSystem = CreateDefaultSubobject<ULockOnComponent>(TEXT("LockOnComponent"));
}

void ASlashCharacter::BeginPlay()
{
	Super::BeginPlay();

	Tags.Add(FName("AttackableCharacter"));
	Tags.Add(FName("Character_CanPickup"));

	InitCameraController();
	InitMappingContext();
	InitHUD();
	InitLockOnComponent();
}

/*
* Initialization 
*/

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
		if (EnhancedInputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			EnhancedInputSubsystem->AddMappingContext(ActionsMappingContext, 0);
			EnhancedInputSubsystem->AddMappingContext(LookMappingContext, 0);
		}
	}
}

void ASlashCharacter::InitHUD()
{
	APlayerController* SlashController = Cast<APlayerController>(GetController());
	if (SlashController)
	{
		HUD = Cast<ASlashHUD>(SlashController->GetHUD());

		HUD->GetSlashOverlay()->SetHealthPercentage(1.f);
		HUD->GetSlashOverlay()->SetStaminaPercentage(1.f);
		HUD->GetSlashOverlay()->SetCoinsAmount(0);
		HUD->GetSlashOverlay()->SetSoulsAmount(0);
	}
}

void ASlashCharacter::InitLockOnComponent()
{
	LockOnSystem->Init(this, ViewCamera);
}

/*
* Input
*/

void ASlashCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Look);
		EnhancedInputComponent->BindAction(EquipAction, ETriggerEvent::Completed, this, &ASlashCharacter::Equip);
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Completed, this, &ASlashCharacter::Attack);
		EnhancedInputComponent->BindAction(DodgeForwardAction, ETriggerEvent::Completed, this, &ASlashCharacter::DodgeForward);

		EnhancedInputComponent->BindAction(ToggleWalkAction, ETriggerEvent::Completed, this, &ASlashCharacter::ToggleWalk);
		EnhancedInputComponent->BindAction(ArmDisarmAction, ETriggerEvent::Completed, this, &ASlashCharacter::ArmDisarm);

		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);

		EnhancedInputComponent->BindAction(LockOnEnableDisableAction, ETriggerEvent::Completed, this, &ASlashCharacter::LockOnEnableDisable);
		EnhancedInputComponent->BindAction(LockOnSwapTargetAction, ETriggerEvent::Completed, this, &ASlashCharacter::LockOnSwapTarget);
	}
}

void ASlashCharacter::DisableAllInput()
{
	EnhancedInputSubsystem->RemoveMappingContext(ActionsMappingContext);
}

void ASlashCharacter::Move(const FInputActionValue& Value)
{	
	if (IsAttacking()) return;

	const FVector2D DirectionalVector = Value.Get<FVector2D>();
	if (Controller)
	{
		ControllerDirectionDeg = UKismetMathLibrary::DegAtan2(DirectionalVector.Y, DirectionalVector.X);

		if (bIsLockOnEnabled && LockOnSystem)
			LockOnSystem->UpdateMovementVector(ControllerDirectionDeg);

		const FVector forwardDirection = FVector::CrossProduct(ViewCamera->GetRightVector(), FVector::UpVector);
		const FVector rightDirection = ViewCamera->GetRightVector();
		AddMovementInput(forwardDirection, DirectionalVector.X);
		AddMovementInput(rightDirection, DirectionalVector.Y);

		// This is the solution presented by Stephen (the result is the same). I like my solution (this one's maybe better in terms of performance)
		/*const FRotator ControlRotation = GetControlRotation();
		const FRotator YawRotation(0.f, ControlRotation.Yaw, 0.f);

		const FVector forwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(forwardDirection, DirectionalVector.X);

		const FVector rightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(rightDirection, DirectionalVector.Y);*/
	}
}

void ASlashCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D Amount = Value.Get<FVector2D>();
	if (Controller)
	{
		AddControllerYawInput(Amount.X);
		AddControllerPitchInput(Amount.Y);
	}
}

void ASlashCharacter::Equip()
{
	 TObjectPtr<AWeapon> WeaponToEquip = Cast<AWeapon>(OverlappingItem);
	 if (WeaponToEquip)
	 {
		 WeaponToEquip->Equip(this->GetMesh(), FName("socket_rightHand"), this, this);
		 Weapon = WeaponToEquip;

		 EquipState = EEquipState::EES_ArmedOneHandedWeapon;

		 UpdateMaxGroundSpeed();
	 }
}

void ASlashCharacter::ToggleWalk()
{
	bWalking = !bWalking;

	UpdateMaxGroundSpeed();
}

void ASlashCharacter::UpdateMaxGroundSpeed()
{
	if (!Attributes) return;

	if (bWalking)
	{
		if (CanGoMaxSpeed())
			GetCharacterMovement()->MaxWalkSpeed = Attributes->GetWalkingSpeedUnequipped();
		else
			GetCharacterMovement()->MaxWalkSpeed = Attributes->GetWalkingSpeedEquipped();
	}
	else
	{
		if (CanGoMaxSpeed())
			GetCharacterMovement()->MaxWalkSpeed = Attributes->GetRunningSpeedUnequipped();
		else
			GetCharacterMovement()->MaxWalkSpeed = Attributes->GetRunningSpeedEquipped();
	}
}

bool ASlashCharacter::CanGoMaxSpeed()
{
	return IsUnequipped() || IsDisarmed();
}

/** Arm/Disarm */

void ASlashCharacter::ArmDisarm()
{
	if (CanArmDisarm())
	{
		ActionState = EActionState::EAS_ArmingDisarming;

		if (IsArmedOneHandedWeapon())
			PlayAnimMontage(ArmDisarmMontage, 1.f, FName("Disarm"));
		else
			PlayAnimMontage(ArmDisarmMontage, 1.f, FName("Arm"));
	}
}

bool ASlashCharacter::CanArmDisarm()
{
	return ArmDisarmMontage &&
		IsUnoccupied() &&
		Weapon;
}

void ASlashCharacter::Arm()
{
	EquipState = EEquipState::EES_ArmedOneHandedWeapon;
	ActionState = EActionState::EAS_Unoccupied;
	UpdateMaxGroundSpeed();
}

void ASlashCharacter::Disarm()
{
	EquipState = EEquipState::EES_Disarmed;
	ActionState = EActionState::EAS_Unoccupied;
	UpdateMaxGroundSpeed();
}

void ASlashCharacter::AttachWeaponToBack()
{
	Weapon->AttachToSocket(this->GetMesh(), FName("socket_weaponBack"));
}

void ASlashCharacter::AttachWeaponToHand()
{
	Weapon->AttachToSocket(this->GetMesh(), FName("socket_rightHand"));
}

/** Jump */

void ASlashCharacter::Jump()
{
	if (CanJump())
		Super::Jump();
}

bool ASlashCharacter::CanJump()
{
	return IsUnoccupied();
}

/** LockOn */

void ASlashCharacter::LockOnEnableDisable()
{
	if (LockOnSystem)
	{
		if (!bIsLockOnEnabled)
		{
			if (LockOnSystem->Enable() > 0)
			{
				bIsLockOnEnabled = true;

				GetCharacterMovement()->bOrientRotationToMovement = false;
			}
		}
		else
		{
			LockOnSystem->Disable();
			bIsLockOnEnabled = false;

			GetCharacterMovement()->bOrientRotationToMovement = true;
		}
	}
}

void ASlashCharacter::LockOnSwapTarget()
{
	if (LockOnSystem)
		LockOnSystem->SwapTarget();
}

/** Dodge */

void ASlashCharacter::DodgeForward()
{
	if (CanDodge())
	{
		ActionState = EActionState::EAS_Dodging;

		ConsumeStamina(DodgeStaminaConsumption);
		DisableCollisionsForDodge();

		FaceCurrentControllerDirection();
		LockOnSystem->Pause();
		PlayMontage(DodgeForwardMontage, DodgeForwardMontage->GetSectionName(0));
	}
}

bool ASlashCharacter::CanDodge()
{
	return DodgeForwardMontage &&
		!IsDodging() &&
		HasEnoughStaminaToDodge();
}

bool ASlashCharacter::HasEnoughStaminaToDodge()
{
	return Attributes? Attributes->GetStamina() > DodgeStaminaConsumption : false;
}

void ASlashCharacter::ConsumeStamina(float StaminaConsumption)
{
	if (Attributes && HUD)
	{
		Attributes->ConsumeStamina(StaminaConsumption);
		HUD->GetSlashOverlay()->SetStaminaPercentage(Attributes->GetStaminaPercent());
	}
}

void ASlashCharacter::DisableCollisionsForDodge()
{
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Ignore);
}

void ASlashCharacter::FaceCurrentControllerDirection()
{
	FVector CameraForward = ViewCamera->GetForwardVector().GetSafeNormal2D();
	FVector ActorForward = GetActorForwardVector().GetSafeNormal2D();

	float ActorFacingDirection = UKismetMathLibrary::DegAcos(FVector::DotProduct(CameraForward, ActorForward));
	float RightOrLeft = FVector::CrossProduct(CameraForward, ActorForward).Z > 0 ? 1.f : -1.f;
	ActorFacingDirection *= RightOrLeft;

	float DegreesFromCurrentFacingToController = ControllerDirectionDeg - ActorFacingDirection;
	AddActorWorldRotation(FRotator(0.f, DegreesFromCurrentFacingToController, 0.f));
}

void ASlashCharacter::DodgeEnd()
{
	ActionState = EActionState::EAS_Unoccupied;

	EnableBackCollisionsAfterDodge();
	LockOnSystem->Resume();
}

void ASlashCharacter::EnableBackCollisionsAfterDodge()
{
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Block);
}

/*
* Hit / Take damage 
*/

float ASlashCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

void ASlashCharacter::GetHit_Implementation(const FVector& HitPoint)
{
	if (HasSomeHealthRemaining())
	{
		AttackEnd();
		ActionState = EActionState::EAS_HitReacting;
	}

	Super::GetHit_Implementation(HitPoint);
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

void ASlashCharacter::Die_Implementation()
{
	ActionState = EActionState::EAS_Dead;

	DisableCollisionsToDie();
	DisableAllInput();
	DisableAttributesRegen();

	Tags.Remove(FName("AttackableCharacter"));

	PlayDeathMontage();
}

void ASlashCharacter::DisableCollisionsToDie()
{
	Super::DisableCollisionsToDie();

	GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Ignore);
}

int16 ASlashCharacter::PlayDeathMontage()
{
	int16 PlayedDeathIndex = Super::PlayDeathMontage();
	TEnumAsByte<EDeathPose> PlayedDeathPose(PlayedDeathIndex);
	if (PlayedDeathPose < GetDeathMontageNumberOfSections())
		DeathPose = PlayedDeathPose;

	return PlayedDeathIndex;
}

/*
* Attack
*/

bool ASlashCharacter::CanAttack()
{
	return IsArmedOneHandedWeapon() && IsUnoccupied();
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

/*
* Pickup 
*/

void ASlashCharacter::SetOverlappingItem_Implementation(AItem* Item)
{
	OverlappingItem = Item;
}

void ASlashCharacter::PickupTreasure_Implementation(AItem* Item)
{
	ATreasure* Treasure = Cast<ATreasure>(Item);
	if (Treasure && Attributes && HUD)
	{
		Attributes->AddGold(Treasure->GetGoldValue());
		HUD->GetSlashOverlay()->SetCoinsAmount(Attributes->GetGoldAmount());
	}
}

void ASlashCharacter::PickupSoul_Implementation(AItem* Item)
{
	ASoul* Soul = Cast<ASoul>(Item);
	if (Soul && Attributes && HUD)
	{
		Attributes->AddSouls(Soul->GetSoulAmount());
		HUD->GetSlashOverlay()->SetSoulsAmount(Attributes->GetSoulsAmount());
	}
}

/*
* General common methods
*/

bool ASlashCharacter::IsUnoccupied()
{
	return ActionState == EActionState::EAS_Unoccupied;
}

bool ASlashCharacter::IsHitReacting()
{
	return ActionState == EActionState::EAS_HitReacting;
}

bool ASlashCharacter::IsDodging()
{
	return ActionState == EActionState::EAS_Dodging;
}

bool ASlashCharacter::IsAttacking()
{
	return ActionState == EActionState::EAS_Attacking;
}

bool ASlashCharacter::IsDead()
{
	return ActionState == EActionState::EAS_Dead;
}

bool ASlashCharacter::IsUnequipped()
{
	return EquipState == EEquipState::EES_Unequipped;
}

bool ASlashCharacter::IsEquipped()
{
	return !IsUnequipped();
}

bool ASlashCharacter::IsDisarmed()
{
	return EquipState == EEquipState::EES_Disarmed;
}

bool ASlashCharacter::IsArmedOneHandedWeapon()
{
	return EquipState == EEquipState::EES_ArmedOneHandedWeapon;
}

void ASlashCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Attributes)
		RecoverStamina(DeltaTime * Attributes->GetStaminaRecoveryRate());
}

void ASlashCharacter::RecoverStamina(float RecoverAmount)
{
	if (Attributes && !Attributes->IsStaminaMaxedOut())
	{
		Attributes->RecoverStamina(RecoverAmount);
		UpdateStaminaBar();
	}
}

void ASlashCharacter::UpdateHealthBar()
{
	if (HUD && Attributes)
		HUD->GetSlashOverlay()->SetHealthPercentage(Attributes->GetHealthPercent());
}

void ASlashCharacter::UpdateStaminaBar()
{
	if (HUD && Attributes)
		HUD->GetSlashOverlay()->SetStaminaPercentage(Attributes->GetStaminaPercent());
}