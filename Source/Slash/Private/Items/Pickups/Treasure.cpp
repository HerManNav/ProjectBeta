// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Pickups/Treasure.h"
#include "Characters/SlashCharacter.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"

ATreasure::ATreasure()
{
	PrimaryActorTick.bCanEverTick = true;

	Sphere->SetGenerateOverlapEvents(false);
}

void ATreasure::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	TObjectPtr<ASlashCharacter> slashCharacter = Cast<ASlashCharacter>(OtherActor);
	if (slashCharacter)
	{
		if (pickupSound)
			UGameplayStatics::PlaySoundAtLocation(this, pickupSound, GetActorLocation());

		Destroy();
	}
}
