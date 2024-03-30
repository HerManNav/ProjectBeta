// Fill out your copyright notice in the Description page of Project Settings.


#include "Pawns/Bird.h"

#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"

#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"

ABird::ABird()
{
	PrimaryActorTick.bCanEverTick = true;

	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	Capsule->SetCapsuleHalfHeight(20.f);
	Capsule->SetCapsuleRadius(15.f);
	SetRootComponent(Capsule);						// Same as RootComponent = capsule, but safer because RootComponent member could become private in future versions of UE

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponent"));
	Mesh->SetupAttachment(GetRootComponent());		// Same as using 'RootComponent' or 'capsule' in the parameter

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(Capsule);			// Same as using 'RootComponent' or 'GetRootComponent()' in the parameter
	SpringArm->TargetArmLength = 300.f;
	SpringArm->AddLocalRotation(FRotator(-15.f, 0.f, 0.f));

	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
	ViewCamera->SetupAttachment(SpringArm);

	AutoPossessPlayer = EAutoReceiveInput::Player0;
}

// Called to bind functionality to input
void ABird::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// OLD SYSTEM: axis and actions mappings
	/*PlayerInputComponent->BindAxis(FName("MoveForwardBackward"), this, &ABird::moveForward);
	PlayerInputComponent->BindAxis(FName("LookRightLeft"), this, &ABird::lookRightAndLeft);
	PlayerInputComponent->BindAxis(FName("LookUpDown"), this, &ABird::lookUpAndDown);*/

	// NEW SYSTEM: enhanced input
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ABird::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ABird::Look);
	}
}

void ABird::BeginPlay()
{
	Super::BeginPlay();
	
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer())) 
		{
			Subsystem->AddMappingContext(BirdMappingContext, 0);
		}
	}
}

// OLD SYSTEM: axis and actions mappings
void ABird::MoveForward(float Value)
{
	if (Controller && (Value != 0.f))	// Check if this pawn is possessed by a controller, otherwise we shouldn't move this pawn.
	{
		FVector	forward = GetActorForwardVector();
		AddMovementInput(forward, Value);
	}
}

void ABird::LookRightAndLeft(float Value)
{
	AddControllerYawInput(Value);
}

void ABird::LookUpAndDown(float Value)
{
	AddControllerPitchInput(Value);
}

// NEW SYSTEM: enhanced input
void ABird::Move(const FInputActionValue& Value)
{
	const float CurrentValue = Value.Get<float>();
	if (Controller && CurrentValue)			// Check if this pawn is possessed by a controller, otherwise we shouldn't move this pawn.
	{
		UE_LOG(LogTemp, Warning, TEXT("IA_Move triggerred %f!"), CurrentValue);

		FVector	forward = GetActorForwardVector();
		AddMovementInput(forward, CurrentValue);
	}
}

void ABird::Look(const FInputActionValue& Value)
{
	const FVector2D CurrentValue = Value.Get<FVector2D>();
	AddControllerYawInput(CurrentValue.X);		// Don't need to check whether this pawn is possessed by a controller (in the move we wouldn't need it actually: AddMovementInput and AddControllerXXXInput already perform this check inside the function)
	AddControllerPitchInput(CurrentValue.Y);
}

