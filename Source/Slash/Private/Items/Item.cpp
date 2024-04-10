// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Item.h"
#include "Characters/SlashCharacter.h"
#include "Interfaces/PickupInterface.h"

#include "Components/SphereComponent.h"
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"

#include "Slash/DebugMacros.h"

AItem::AItem()
{
	PrimaryActorTick.bCanEverTick = true;

	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	RootComponent = Sphere;

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMeshComponent"));
	ItemMesh->SetupAttachment(GetRootComponent());
	ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Niagara = CreateDefaultSubobject<UNiagaraComponent>(TEXT("niagara"));
	Niagara->SetupAttachment(Sphere);
}

void AItem::BeginPlay()
{
	Super::BeginPlay();

	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnSphereOverlap);
	Sphere->OnComponentEndOverlap.AddDynamic(this, &AItem::OnSphereOverlapEnd);
}

void AItem::AttachToComponentAndSocket(USceneComponent* InParent, const FName& InSocketName)
{
	FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget, true);
	GetRootComponent()->AttachToComponent(InParent, TransformRules, InSocketName);
}

void AItem::DisableSphereCollision()
{
	if (Sphere)
		Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

float AItem::TransformedSin()
{
	return Amplitude * FMath::Sin(RunningTime * TimeConstant);
}

float AItem::TransformedCos()
{
	return Amplitude * FMath::Cos(RunningTime * TimeConstant);
}

void AItem::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!CanActorPickup(OtherActor)) return;

	IPickupInterface* PickupInterface = Cast<IPickupInterface>(OtherActor);
	if (PickupInterface)
		PickupInterface->Execute_SetOverlappingItem(OtherActor, this);
}

void AItem::OnSphereOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!CanActorPickup(OtherActor)) return;

	IPickupInterface* PickupInterface = Cast<IPickupInterface>(OtherActor);
	if (PickupInterface)
		PickupInterface->Execute_SetOverlappingItem(OtherActor, nullptr);
}

bool AItem::CanActorPickup(AActor* Actor)
{
	return Actor->ActorHasTag(FName("Character_CanPickup"));
}


void AItem::SetGenerateOverlapEvents(bool enable)
{
	Sphere->SetGenerateOverlapEvents(enable);
}

void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	RunningTime += DeltaTime;

	if (ItemState == EItemState::EIS_Hovering)
	{
		AddActorWorldOffset(FVector(0.f, 0.f, TransformedSin()));
	}
}

/*
* Sound
*/

void AItem::PlaySound(USoundBase* Sound, FVector Location)
{
	if (Sound)
		UGameplayStatics::PlaySoundAtLocation(this, Sound, Location);
}

void AItem::PlayPickupSound()
{
	PlaySound(PickupSound, GetActorLocation());
}

/*
* Particles
*/

void AItem::PlayPickupParticles()
{

}
