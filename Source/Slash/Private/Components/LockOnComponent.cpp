// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/LockOnComponent.h"

#include "Camera/CameraComponent.h"

#include "Kismet/KismetSystemLibrary.h"

ULockOnComponent::ULockOnComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void ULockOnComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerLocation = GetOwner()->GetActorLocation();
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
	GetLockOnTargets(Targets);
	if (Targets.Num() > 0)
	{
		CurrentTargetIndex = 0;
		DistanceToCurrentEnemy = GetDistanceToEnemyAtIndex(Targets, CurrentTargetIndex);
	}

	return Targets.Num();
}

void ULockOnComponent::GetLockOnTargets(TArray<AActor*>& InTargets)
{
	TArray<FHitResult> PawnHits;
	GetHitsToPawnsInFront(PawnHits);

	InTargets = GetVisibleImpactedActors(PawnHits);
}

void ULockOnComponent::GetHitsToPawnsInFront(TArray<FHitResult> PawnHits)
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
		PawnHits,
		true
	);
}

TArray<AActor*> ULockOnComponent::GetVisibleImpactedActors(TArray<FHitResult> PawnHits)
{
	TArray<AActor*> VisiblePawns;

	for (FHitResult PawnHit : PawnHits)
	{	
		FHitResult HitResult;
		DoSphereTrace(PawnHit.ImpactPoint, HitResult);

		if (!HitResult.bBlockingHit)
			VisiblePawns.Add(PawnHit.GetActor());
	}

	return VisiblePawns;
}

void ULockOnComponent::DoSphereTrace(const FVector& PawnHitLocation, FHitResult& HitResult)
{
	FVector TraceStart = ViewCamera->GetComponentLocation();
	FVector TraceEnd = PawnHitLocation;
	float Radius = 1.f;

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic));

	TArray<AActor*> ActorsToIgnore;

	UKismetSystemLibrary::SphereTraceSingleForObjects(
		this,
		TraceStart, TraceEnd,
		Radius,
		ObjectTypes, false,
		ActorsToIgnore,
		bDrawDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
		HitResult,
		true
	);
}

float ULockOnComponent::GetDistanceToEnemyAtIndex(const TArray<AActor*>& Enemies, int16 Index)
{
	return 0.0f;
}

void ULockOnComponent::Disable()
{
}

void ULockOnComponent::SwapTarget()
{
}

