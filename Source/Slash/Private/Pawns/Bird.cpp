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

	capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("capsule"));
	capsule->SetCapsuleHalfHeight(20.f);
	capsule->SetCapsuleRadius(15.f);
	SetRootComponent(capsule);						// Same as RootComponent = capsule, but safer because RootComponent member could become private in future versions of UE

	mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("meshComponent"));
	mesh->SetupAttachment(GetRootComponent());		// Same as using 'RootComponent' or 'capsule' in the parameter

	springArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("springArm"));
	springArm->SetupAttachment(capsule);			// Same as using 'RootComponent' or 'GetRootComponent()' in the parameter
	springArm->TargetArmLength = 300.f;
	springArm->AddLocalRotation(FRotator(-15.f, 0.f, 0.f));

	viewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("viewCamera"));
	viewCamera->SetupAttachment(springArm);

	AutoPossessPlayer = EAutoReceiveInput::Player0;
}

void ABird::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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
	if (UEnhancedInputComponent* enhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		enhancedInputComponent->BindAction(moveAction, ETriggerEvent::Triggered, this, &ABird::move);
		enhancedInputComponent->BindAction(lookAction, ETriggerEvent::Triggered, this, &ABird::look);
	}
}

void ABird::BeginPlay()
{
	Super::BeginPlay();
	
	if (APlayerController* playerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(playerController->GetLocalPlayer())) 
		{
			subsystem->AddMappingContext(birdMappingContext, 0);
		}
	}
}

// OLD SYSTEM: axis and actions mappings
void ABird::moveForward(float value)
{
	if (Controller && (value != 0.f))	// Check if this pawn is possessed by a controller, otherwise we shouldn't move this pawn.
	{
		FVector	forward = GetActorForwardVector();
		AddMovementInput(forward, value);
	}
}

void ABird::lookRightAndLeft(float value)
{
	AddControllerYawInput(value);
}

void ABird::lookUpAndDown(float value)
{
	AddControllerPitchInput(value);
}

// NEW SYSTEM: enhanced input
void ABird::move(const FInputActionValue& value)
{
	const float currentValue = value.Get<float>();
	if (Controller && currentValue)			// Check if this pawn is possessed by a controller, otherwise we shouldn't move this pawn.
	{
		UE_LOG(LogTemp, Warning, TEXT("IA_Move triggerred %f!"), currentValue);

		FVector	forward = GetActorForwardVector();
		AddMovementInput(forward, currentValue);
	}
}

void ABird::look(const FInputActionValue& value)
{
	const FVector2D currentValue = value.Get<FVector2D>();
	AddControllerYawInput(currentValue.X);		// Don't need to check whether this pawn is possessed by a controller (in the move we wouldn't need it actually: AddMovementInput and AddControllerXXXInput already perform this check inside the function)
	AddControllerPitchInput(currentValue.Y);
}

