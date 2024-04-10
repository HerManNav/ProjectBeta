// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Pickups/Treasure.h"
#include "Interfaces/PickupInterface.h"

#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"

ATreasure::ATreasure()
{
	PrimaryActorTick.bCanEverTick = true;

	Sphere->SetGenerateOverlapEvents(false);
}

void ATreasure::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!CanActorPickup(OtherActor)) return;

	IPickupInterface* PickupInterface = Cast<IPickupInterface>(OtherActor);
	if (PickupInterface)
	{
		PickupInterface->Execute_PickupTreasure(OtherActor, this);
		
		PlayPickupSound();
		Destroy();
	}
}