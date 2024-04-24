// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/LockOnComponent.h"
#include "Interfaces/LockOnInterface.h"

#include "Camera/CameraComponent.h"

#include "Kismet/KismetSystemLibrary.h"

ULockOnComponent::ULockOnComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void ULockOnComponent::BeginPlay()
{
	Super::BeginPlay();
}

void ULockOnComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void ULockOnComponent::Init(UCameraComponent* Camera)
{
	ViewCamera = Camera;
}

int16 ULockOnComponent::Enable()
{
	if (!bActivate) return 0;

	OwnerLocation = GetOwner()->GetActorLocation();

	GetLockOnTargets(Targets);
	if (Targets.Num() > 0)
	{
		CurrentTargetIndex = 0;
		CurrentTarget = Targets[CurrentTargetIndex];
		ShowLockOnWidgetOnActor(CurrentTarget);

		DistanceToCurrentEnemy = GetDistanceToTarget(CurrentTarget);
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
	return (Target->GetActorLocation() - OwnerLocation).Size();
}

/** Disable */

void ULockOnComponent::Disable()
{
	if (!bActivate) return;

	Targets.Empty();
	CurrentTargetIndex = 0;

	HideLockOnWidgetOnActor(CurrentTarget);
	StopRotationTimeline();
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
	if (!bActivate) return;

}

