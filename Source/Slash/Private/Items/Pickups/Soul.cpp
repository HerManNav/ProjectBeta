// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Pickups/Soul.h"
#include "Interfaces/PickupInterface.h"

ASoul::ASoul()
{
	PrimaryActorTick.bCanEverTick = false;
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