// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Weapons/Weapon.h"
#include "Characters/SlashCharacter.h"
#include "Interfaces/HitInterface.h"

#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "NiagaraComponent.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

AWeapon::AWeapon()
{
	WeaponBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Weapon Box"));
	WeaponBox->SetupAttachment(GetRootComponent());
	WeaponBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);		// We'd activate collision only when needed (see setBoxCollision())
	WeaponBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	WeaponBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

	BoxTraceStart = CreateDefaultSubobject<USceneComponent>(TEXT("Box Trace Start"));
	BoxTraceStart->SetupAttachment(GetRootComponent());

	BoxTraceEnd = CreateDefaultSubobject<USceneComponent>(TEXT("Box Trace End"));
	BoxTraceEnd->SetupAttachment(GetRootComponent());
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	WeaponBox->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnBoxOverlap);
}

void AWeapon::Equip(USceneComponent* Parent, FName SocketName, AActor* NewOwner, APawn* NewInstigator)
{
	SetOwner(NewOwner);
	SetInstigator(NewInstigator);

	AttachToComponentAndSocket(Parent, SocketName);
	PlaySound(EquipSound, GetActorLocation());
	DisableSphereCollision();

	ItemState = EItemState::EIS_Equipped;
}

void AWeapon::DeactivateNiagara()
{
	Niagara->Deactivate();
}

void AWeapon::SetBoxCollision(ECollisionEnabled::Type CollisionEnabled)
{
	if (WeaponBox) WeaponBox->SetCollisionEnabled(CollisionEnabled);
}

void AWeapon::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	FHitResult BoxHit;
	PerformBoxTrace(BoxHit);

	if (BoxHit.bBlockingHit)	// Same than boxHit.GetActor()
	{
		ApplyDamage(BoxHit);
		ApplyHit(BoxHit);

		ActorsToIgnore.AddUnique(BoxHit.GetActor());

		CreateHitFields(BoxHit.ImpactPoint);
	}
}

void AWeapon::PerformBoxTrace(FHitResult& BoxHit)
{
	ActorsToIgnore.Add(this);

	const FVector Start = BoxTraceStart->GetComponentLocation();
	const FVector End = BoxTraceEnd->GetComponentLocation();

	UKismetSystemLibrary::BoxTraceSingle(	this,
											Start, End, 
											FVector(5.f, 5.f, 5.f), BoxTraceStart->GetComponentRotation(),
											ETraceTypeQuery::TraceTypeQuery1, false,
											ActorsToIgnore,
											bShowDebugBoxTrace ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
											BoxHit,
											true);
}

void AWeapon::ApplyDamage(const FHitResult& BoxHit)
{
	/** This will trigger AActor::TakeDamage(), which is overrided for classes that actually should take damage, like AEnemy.
	*	This is priority over executing GetHit(), bc Enemy's health needs to be updated in order for GetHit to know whether to play Hit or Death animation.
	*/

	float Damage = FMath::FRandRange(BaseDamage - DamageVariation, BaseDamage + DamageVariation);
	UGameplayStatics::ApplyDamage(BoxHit.GetActor(), Damage, GetInstigator()->GetController(), this, UDamageType::StaticClass());
}

void AWeapon::ApplyHit(const FHitResult& BoxHit)
{
	/** Checking if the hit actor implements the hit Interface (not all actors would need to execute logics when hit, like sounds, particles, etc.).
	*   Typically, here one would execute all that logic unrelated to applying damage.
	*/

	IHitInterface* HitInterface = Cast<IHitInterface>(BoxHit.GetActor());
	if (HitInterface)
		HitInterface->Execute_GetHit(BoxHit.GetActor(), BoxHit.ImpactPoint);
}