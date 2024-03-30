// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/BaseCharacter.h"
#include "Items/Weapons/Weapon.h"

#include "Components/AttributesComponent.h"

#include "Kismet/GameplayStatics.h"
//#include "Kismet/KismetSystemLibrary.h"

//#include "Slash/DebugMacros.h"

ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	Attributes = CreateDefaultSubobject<UAttributesComponent>(TEXT("attributes"));

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
}

void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (AttackMontageSectionNames.Num() <= 0)
		UE_LOG(LogTemp, Warning, TEXT("No montage sections specified for Attack montage. No animation will be played."));
	if (DeathMontageSectionNames.Num() <= 0)
		UE_LOG(LogTemp, Warning, TEXT("No montage sections specified for Death montage. No animation will be played."));
}

/*
* Montages
*/

int16 ABaseCharacter::PlayAttackingMontage()
{
	return PlayRandomMontageSection(AttackMontage, AttackMontageSectionNames, AttackIndex);
}

int16 ABaseCharacter::PlayDeathMontage()
{
	return PlayRandomMontageSection(deathMontage, DeathMontageSectionNames, DeathIndex);
}

void ABaseCharacter::PlayMontage(UAnimMontage* Montage, FName MontageName)
{
	TObjectPtr<UAnimInstance> AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && Montage)
	{
		AnimInstance->Montage_Play(Montage, 1.f);
		AnimInstance->Montage_JumpToSection(MontageName, Montage);
	}
}

int16 ABaseCharacter::PlayRandomMontageSection(UAnimMontage* Montage, TArray<FName> MontageSectionNames, int8 SectionIndexDefault)
{
	bool bCanPlayMontageSection =	Montage && 
									MontageSectionNames.Num() > 0 &&
									-1 <= SectionIndexDefault && SectionIndexDefault < MontageSectionNames.Num();
	if (bCanPlayMontageSection)
	{
		int16 SectionIndex = SectionIndexDefault;
		if (SectionIndexDefault == -1)
			SectionIndex = FMath::RandRange(0, MontageSectionNames.Num() - 1);

		PlayMontage(Montage, MontageSectionNames[SectionIndex]);
		return SectionIndex;
	}
	
	UE_LOG(LogTemp, Warning, TEXT("Animation montage section index not available, not playing animation"))
	return -1;
}

/* 
* Attack
*/

void ABaseCharacter::ReactToHitBasedOnHitDirection(const FVector& HitPoint)
{
	FName DirectionName = GetHitDirection(HitPoint);
	PlayMontage(HitMontage, DirectionName);
}

FName ABaseCharacter::GetHitDirection(const FVector& HitPoint)
{
	FVector ActorForwardVector = GetActorForwardVector().GetSafeNormal();
	FVector HitVector = (FVector(HitPoint.X, HitPoint.Y, GetActorLocation().Z) - GetActorLocation()).GetSafeNormal();	// We couldn't do "hitVector = (hitPoint - GetActorLocation()).GetSafeNormal(); hitVector.Z = actorForwardVector.Z;" because of the normalization. We should do instead "hitVector = (hitPoint - GetActorLocation()); hitVector.Z = actorForwardVector.Z; hitVector = hitVector.GetSafeNormal();", which is equivalent to this line here

	// dotProd(afv, hv) = |afv||hv| * cos(theta) ; but |afv| = |hv| = 1 since they are normalized vectors, so dotProd(afv, hv) = cos(theta)
	float CosTheta = FVector::DotProduct(ActorForwardVector, HitVector);
	float AngleInRadians = acos(CosTheta);
	float AngleInDegrees = FMath::RadiansToDegrees(AngleInRadians);

	/* DEBUG */
	/*DRAW_SPHERE(hitPoint, 10.f);
	UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(), GetActorLocation() + 100.f * actorForwardVector, 5.f, FColor::Cyan, 10.f);
	UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(), GetActorLocation() + 100.f * hitVector, 5.f, FColor::Green, 10.f);

	UE_LOG(LogTemp, Warning, TEXT("degrees %f"), angleInDegrees);
	UE_LOG(LogTemp, Warning, TEXT("degrees %f"), FVector::CrossProduct(actorForwardVector, hitVector).Z);*/
	/* END DEBUG */

	if (0.f < AngleInDegrees && AngleInDegrees < FrontBackAngle)
		return FName("HitFromFront");
	else if ((180.f - FrontBackAngle) < AngleInDegrees && AngleInDegrees < 180.f)
		return FName("HitFromBack");
	else
	{
		// The crossProd for 2 vectors in the same plane is a perpendicular one (positive or negative). Components X & Y are 0, and in Z it is the magnitude of the resulting product
		float FromLeftOrRight = FVector::CrossProduct(ActorForwardVector, HitVector).Z;
		if (FromLeftOrRight < 0.f)
			return FName("HitFromLeft");
		else
			return FName("HitFromRight");
	}
}

void ABaseCharacter::SetWeaponCollision(ECollisionEnabled::Type CollisionEnabled)
{
	if (Weapon)
	{
		Weapon->SetBoxCollision(CollisionEnabled);
		Weapon->ClearActorsToIgnore();
	}
}

/*
* Sound and VFX
*/

void ABaseCharacter::PlayHitSoundAtLocation(const FVector& Location)
{
	if (HitSound) UGameplayStatics::PlaySoundAtLocation(this, HitSound, Location);
}

void ABaseCharacter::PlayHitParticlesAtLocation(const FVector& Location)
{
	if (HitVFX) UGameplayStatics::SpawnEmitterAtLocation(this, HitVFX, Location);
}