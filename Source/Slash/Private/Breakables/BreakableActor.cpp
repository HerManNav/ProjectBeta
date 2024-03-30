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

	GeometryCollection = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("GeometryCollection"));
	SetRootComponent(GeometryCollection);

	GeometryCollection->SetGenerateOverlapEvents(true);
	GeometryCollection->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GeometryCollection->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

	CollisionCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CollisionCapsule"));
	CollisionCapsule->SetupAttachment(RootComponent);

	CollisionCapsule->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	CollisionCapsule->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	bProcessedOnChaosBreakEvent = false;
}

void ABreakableActor::BeginPlay()
{
	Super::BeginPlay();

	GeometryCollection->SetNotifyBreaks(true);
	GeometryCollection->OnChaosBreakEvent.AddDynamic(this, &ABreakableActor::OnChaosBreakEvent);
}

void ABreakableActor::OnChaosBreakEvent(const FChaosBreakEvent& BreakEvent)
{
	if (!bProcessedOnChaosBreakEvent)
	{
		SetLifeSpan(30.0f);
		
		if (BreakingSound)
			UGameplayStatics::PlaySoundAtLocation(this, BreakingSound, GetActorLocation());

		spawnRandomTreasure(BreakEvent.Location);

		CollisionCapsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		bProcessedOnChaosBreakEvent = true;
	}
}

void ABreakableActor::spawnRandomTreasure(const FVector& Location)
{
	// Spawn ATreasure actor
	if (GetWorld() && ClassesToSpawn.Num() > 0)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		const uint8 TreasureIndex = FMath::RandRange(0, ClassesToSpawn.Num() - 1);
		SpawnedTreasure = GetWorld()->SpawnActor<ATreasure>(ClassesToSpawn[TreasureIndex],
															FVector(Location.X, Location.Y, 75.f),
															FRotator(0.f, 0.f, 0.f),
															SpawnParams);


		// Delay for the player to know there was actually a treasure and only after let the player to grab the treasure 
		FTimerDelegate delegate;
		delegate.BindLambda([&]() { SpawnedTreasure->SetGenerateOverlapEvents(true); });
		FTimerHandle timerHandler;
		GetWorldTimerManager().SetTimer(timerHandler, delegate, 0.2f, false);
	}
}

void ABreakableActor::GetHit_Implementation(const FVector& hitPoint)
{
	// I've moved this logic to onChaosBreakEvent(...)
}
