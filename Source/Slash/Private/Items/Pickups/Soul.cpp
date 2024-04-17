// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Pickups/Soul.h"
#include "Interfaces/PickupInterface.h"

#include "Kismet/KismetSystemLibrary.h"

ASoul::ASoul()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ASoul::BeginPlay()
{
	Super::BeginPlay();

	TargetZ = GetTargetZ();
	SetLifeSpan(Lifespan);
}

float ASoul::GetTargetZ()
{
	FVector TracingStart = GetActorLocation();
	FVector TracingEnd = FVector(TracingStart.X, TracingStart.Y, TracingStart.Z - 1000);
	FHitResult HitResult;

	TArray<AActor*> ActorsToIgnore;
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic));

	UKismetSystemLibrary::LineTraceSingleForObjects(
		this,
		TracingStart, TracingEnd,
		ObjectTypes	, false, ActorsToIgnore,
		EDrawDebugTrace::ForDuration,
		HitResult,
		true);

	FVector HitLocation = HitResult.Location;
	return HitLocation.Z + DesiredTargetAltitude;
}

void ASoul::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!CanActorPickup(OtherActor)) return;

	IPickupInterface* PickupInterface = Cast<IPickupInterface>(OtherActor);
	if (PickupInterface)
	{
		PickupInterface->Execute_PickupSoul(OtherActor, this);

		PlayPickupSound();
		PlayPickupParticles();
		Destroy();
	}
}

void ASoul::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float DeltaZ = DriftingSpeed * DeltaTime;
	if (GetActorLocation().Z > TargetZ)
	{
		FVector Offset(0.f, 0.f, -DeltaZ);
		AddActorWorldOffset(Offset);
	}
}