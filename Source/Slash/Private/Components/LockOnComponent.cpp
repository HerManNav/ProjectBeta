// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/LockOnComponent.h"
#include "Interfaces/LockOnInterface.h"

#include "Camera/CameraComponent.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Character.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"

ULockOnComponent::ULockOnComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	//RotationTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("RotationTimeline"));
	//InterpFunction->BindDynamic(this, &ULockOnComponent::LerpControllerRotation);
}

void ULockOnComponent::BeginPlay()
{
	Super::BeginPlay();

	FOnTimelineFloat InterpFunction;
	InterpFunction.BindUFunction(this, TEXT("LerpControllerRotation"));

	RotationTimeline.AddInterpFloat(TimelineCurve, InterpFunction);
}

void ULockOnComponent::Init(ACharacter* InOwner, UCameraComponent* Camera)
{
	Owner = InOwner;
	ViewCamera = Camera;
}

/** Enable */

int16 ULockOnComponent::Enable()
{
	if (!bActivated) return 0;

	GetLockOnTargets(Targets);
	if (Targets.Num() > 0)
	{
		CurrentTargetIndex = 0;
		CurrentTarget = Targets[CurrentTargetIndex];
		ShowLockOnWidgetOnActor(CurrentTarget);

		RotationTimeline.PlayFromStart();
	}

	UE_LOG(LogTemp, Warning, TEXT("num of targets %d"), Targets.Num())

	return Targets.Num();
}

void ULockOnComponent::GetLockOnTargets(TArray<AActor*>& InTargets)
{
	TArray<FHitResult> PawnHits;

	GetHitsToPawnsInFront(PawnHits);
	GetVisibleImpactedActors(InTargets, PawnHits);
}

void ULockOnComponent::GetHitsToPawnsInFront(TArray<FHitResult>& OutPawnHits)
{
	FVector OwnerLocation = Owner->GetActorLocation();

	FVector TraceStart = OwnerLocation;
	FVector TraceEnd = OwnerLocation + TraceLength * ViewCamera->GetForwardVector().GetSafeNormal2D();
	FVector HalfSize = FVector(500.f, 500.f, 200.f);
	FRotator Orientation = FRotator(0.f, 0.f, 0.f);

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));

	TArray<AActor*> ActorsToIgnore;

	UKismetSystemLibrary::BoxTraceMultiForObjects(
		this,
		TraceStart, TraceEnd,
		HalfSize, Orientation,
		ObjectTypes, false,
		ActorsToIgnore,
		bDrawDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
		OutPawnHits,
		true
	);
}

void ULockOnComponent::GetVisibleImpactedActors(TArray<AActor*>& OutVisiblePawns, const TArray<FHitResult>& PawnHits)
{
	for (FHitResult PawnHit : PawnHits)
	{	
		FHitResult HitResult;
		DoLineTraceAgainstWorldStatic(
			HitResult, 
			ViewCamera->GetComponentLocation(), PawnHit.ImpactPoint);

		if (!HitResult.bBlockingHit)
			OutVisiblePawns.Add(PawnHit.GetActor());
	}
}

void ULockOnComponent::DoLineTraceAgainstWorldStatic(FHitResult& OutHitResult, const FVector& TraceStart, const FVector& TraceEnd)
{
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic));

	TArray<AActor*> ActorsToIgnore;

	UKismetSystemLibrary::LineTraceSingleForObjects(
		this,
		TraceStart, TraceEnd,
		ObjectTypes, false,
		ActorsToIgnore,
		bDrawDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
		OutHitResult,
		true
	);
}

float ULockOnComponent::GetDistanceToTarget(const AActor* Target)
{
	return (Target->GetActorLocation() - Owner->GetActorLocation()).Size();
}

/** Disable */

void ULockOnComponent::Disable()
{
	if (!bActivated) return;

	Targets.Empty();
	CurrentTargetIndex = 0;

	HideLockOnWidgetOnActor(CurrentTarget);
	StopRotationTimeline();

	RotationTimeline.Stop();
}

void ULockOnComponent::ShowLockOnWidgetOnActor(AActor* Actor)
{
	ILockOnInterface* LockOnInterface = Cast<ILockOnInterface>(Actor);
	if (LockOnInterface)
		LockOnInterface->SetLockOnWidgetVisibility(true);
}

void ULockOnComponent::HideLockOnWidgetOnActor(AActor* Actor)
{
	ILockOnInterface* LockOnInterface = Cast<ILockOnInterface>(Actor);
	if (LockOnInterface)
		LockOnInterface->SetLockOnWidgetVisibility(false);
}

void ULockOnComponent::StopRotationTimeline()
{
}

/** Swap target */

void ULockOnComponent::SwapTarget()
{
	if (!bActivated) return;

	//Swap CurrentTarget
}

/** Tick */

void ULockOnComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	if (!bActivated) return;

	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (Targets.Num() > 0)
	{
		DistanceToCurrentEnemy = GetDistanceToTarget(CurrentTarget);
		if (DistanceToCurrentEnemy > DistanceToDeactivateLockOn)
		{
			RotationTimeline.TickTimeline(DeltaTime);
		}
	}
}

/** Focus target */

void ULockOnComponent::LerpControllerRotation(float Alpha)
{
	//UE_LOG(LogTemp, Warning, TEXT("inside lerp, alpha %f"), Alpha);

	AController* OwnerController = Owner->GetController();
	FRotator CurrentControllerRotation = OwnerController->GetControlRotation();
	FRotator FocusToTargetRotation = GetFocusToTargetRotation();

	FRotator LerpedRotation = UKismetMathLibrary::RLerp(CurrentControllerRotation, FocusToTargetRotation, Alpha, false);

	Owner->GetController()->SetControlRotation(LerpedRotation);
}

FRotator ULockOnComponent::GetFocusToTargetRotation()
{
	FVector OwnerLocation = Owner->GetActorLocation();

	FVector CameraTilt = FVector(OwnerLocation.X, OwnerLocation.Y, OwnerLocation.Z + CameraTiltCurve->GetFloatValue(DistanceToCurrentEnemy));
	FVector CameraRightDisplacement = FVector(ViewCamera->GetRightVector() * CameraRightDisplacementCurve->GetFloatValue(DistanceToCurrentEnemy));

	FVector CameraLocation = CameraTilt + CameraRightDisplacement;
	FVector CurrentTargetLocation = CurrentTarget->GetActorLocation();

	return UKismetMathLibrary::FindLookAtRotation(CameraLocation, CurrentTargetLocation);
}