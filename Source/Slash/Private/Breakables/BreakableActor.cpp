// Fill out your copyright notice in the Description page of Project Settings.


#include "Breakables/BreakableActor.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetArrayLibrary.h"
#include "Items/Pickups/Treasure.h"

ABreakableActor::ABreakableActor()
{
	PrimaryActorTick.bCanEverTick = false;

	geometryCollection = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("geometryCollection"));
	SetRootComponent(geometryCollection);

	geometryCollection->SetGenerateOverlapEvents(true);
	geometryCollection->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	geometryCollection->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

	collisionCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("collisionCapsule"));
	collisionCapsule->SetupAttachment(RootComponent);

	collisionCapsule->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	collisionCapsule->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	onChaosBreakEvent_processed = false;
}

void ABreakableActor::BeginPlay()
{
	Super::BeginPlay();

	geometryCollection->SetNotifyBreaks(true);
	geometryCollection->OnChaosBreakEvent.AddDynamic(this, &ABreakableActor::onChaosBreakEvent);
}

void ABreakableActor::onChaosBreakEvent(const FChaosBreakEvent& breakEvent)
{
	if (!onChaosBreakEvent_processed)
	{
		SetLifeSpan(30.0f);
		
		if (breakingSound)
			UGameplayStatics::PlaySoundAtLocation(this, breakingSound, GetActorLocation());

		spawnRandomTreasure(breakEvent.Location);

		collisionCapsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		onChaosBreakEvent_processed = true;
	}
}

void ABreakableActor::spawnRandomTreasure(const FVector& location)
{
	// Spawn ATreasure actor
	if (GetWorld() && classesToSpawn.Num() > 0)
	{
		FActorSpawnParameters spawnParams;
		spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		const uint8 treasureIndex = FMath::RandRange(0, classesToSpawn.Num() - 1);
		spawnedTreasure = GetWorld()->SpawnActor<ATreasure>(classesToSpawn[treasureIndex],
															FVector(location.X, location.Y, 75.f),
															FRotator(0.f, 0.f, 0.f),
															spawnParams);


		// Delay for the player to know there was actually a treasure and only after let the player to grab the treasure 
		FTimerDelegate delegate;
		delegate.BindLambda([&]() { spawnedTreasure->setGenerateOverlapEvents(true); });
		FTimerHandle timerHandler;
		GetWorldTimerManager().SetTimer(timerHandler, delegate, 0.2f, false);
	}
}

void ABreakableActor::getHit_Implementation(const FVector& hitPoint)
{
	// I've moved this logic to onChaosBreakEvent(...)
}
