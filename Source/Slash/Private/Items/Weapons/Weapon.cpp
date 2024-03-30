// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Weapons/Weapon.h"
#include "Characters/SlashCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Interfaces/HitInterface.h"
#include "NiagaraComponent.h"

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

void AWeapon::Equip(USceneComponent* parent, FName socketName, AActor* NewOwner, APawn* NewInstigator)
{
	SetOwner(NewOwner);
	SetInstigator(NewInstigator);

	AttachToComponentAndSocket(parent, socketName);

	ItemState = EItemState::EIS_Equipped;

	if (EquipSound)
		UGameplayStatics::PlaySoundAtLocation(this, EquipSound,	GetActorLocation());

	if (Sphere)
		Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Niagara->Deactivate();
}

void AWeapon::AttachToComponentAndSocket(USceneComponent* InParent, const FName& InSocketName)
{
	FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget, true);
	GetRootComponent()->AttachToComponent(InParent, TransformRules, InSocketName);
}

void AWeapon::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}

void AWeapon::OnSphereOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnSphereOverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
}

void AWeapon::SetBoxCollision(ECollisionEnabled::Type CollisionEnabled)
{
	if (WeaponBox) WeaponBox->SetCollisionEnabled(CollisionEnabled);
}

void AWeapon::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	const FVector Start = BoxTraceStart->GetComponentLocation();
	const FVector End = BoxTraceEnd->GetComponentLocation();

	ActorsToIgnore.Add(this);

	FHitResult BoxHit;
	UKismetSystemLibrary::BoxTraceSingle(	this,
											Start, End, FVector(5.f, 5.f, 5.f), BoxTraceStart->GetComponentRotation(),
											ETraceTypeQuery::TraceTypeQuery1, false,
											ActorsToIgnore,
											EDrawDebugTrace::None,
											BoxHit, 
											true);

	if (BoxHit.bBlockingHit)	// would be better boxHit.GetActor() ? (seems that the result is the same)
	{
		// 1st. Apply damage: this will trigger AActor::TakeDamage(...), which is overrided for classes that actually should take damage, like AEnemy.
		// This is priority over executing getHit_Implemation, bc death animation play is implemented in TakeDamage method
		float Damage = FMath::FRandRange(BaseDamage - DamageVariation, BaseDamage + DamageVariation);
		UGameplayStatics::ApplyDamage(BoxHit.GetActor(), Damage, GetInstigator()->GetController(), this, UDamageType::StaticClass());

		// 2nd. Call GetHit_Implementation, which would run the hit animation in case the actor is still alive
		IHitInterface* HitInterface = Cast<IHitInterface>(BoxHit.GetActor());
		if (HitInterface)		// This is equivalent to check whether the actor boxHit.GetActor() is an element that implements the getHit() method, i.e. the actor can get hit and has some logic to be executed when get hit (not necessarily all actors would need to execute logic, like sounds, particles, etc.)
			HitInterface->Execute_GetHit(BoxHit.GetActor(), BoxHit.ImpactPoint);

		ActorsToIgnore.AddUnique(BoxHit.GetActor());

		// 3. Create impact forces for physics
		CreateHitFields(BoxHit.ImpactPoint);	// This would be executed even for non-breakable actors, so maybe near breakable objects are affected when hitting a regular enemy (which seems cool)
	}
}