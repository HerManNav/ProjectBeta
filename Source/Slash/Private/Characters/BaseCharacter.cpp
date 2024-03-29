// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/BaseCharacter.h"
#include "Items/Weapons/Weapon.h"
#include "Components/AttributesComponent.h"

ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	attributes = CreateDefaultSubobject<UAttributesComponent>(TEXT("attributes"));
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (AttackMontageSectionNames.Num() <= 0)
		UE_LOG(LogTemp, Warning, TEXT("No montage sections specified for Attack montage. No animation will be played."));
	if (DeathMontageSectionNames.Num() <= 0)
		UE_LOG(LogTemp, Warning, TEXT("No montage sections specified for Death montage. No animation will be played."));
}

void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

/*
* Montages
*/

void ABaseCharacter::PlayMontage(UAnimMontage* Montage, FName MontageName)
{
	TObjectPtr<UAnimInstance> animInstance = GetMesh()->GetAnimInstance();
	if (animInstance && Montage)
	{
		animInstance->Montage_Play(Montage, 1.f);
		animInstance->Montage_JumpToSection(MontageName, Montage);
	}
}

int16 ABaseCharacter::PlayRandomMontageSection(UAnimMontage* Montage, TArray<FName> MontageSectionNames)
{
	if (Montage && MontageSectionNames.Num() > 0)
	{
		int16 sectionIndex = FMath::RandRange(0, MontageSectionNames.Num() - 1);
		PlayMontage(Montage, MontageSectionNames[sectionIndex]);
		return sectionIndex;
	}
	return -1;
}

int16 ABaseCharacter::PlayAttackingMontage()
{
	return PlayRandomMontageSection(AttackMontage, AttackMontageSectionNames);
}

int16 ABaseCharacter::PlayDeathMontage()
{
	return PlayRandomMontageSection(deathMontage, DeathMontageSectionNames);
}


/* 
* Attack
*/

void ABaseCharacter::setWeaponCollision(ECollisionEnabled::Type collisionEnabled)
{
	if (weapon)
	{
		weapon->setBoxCollision(collisionEnabled);
		weapon->clearActorsToIgnore();
	}
}

FName ABaseCharacter::getHitDirection(const FVector& hitPoint)
{
	FVector actorForwardVector = GetActorForwardVector().GetSafeNormal();
	FVector hitVector = (FVector(hitPoint.X, hitPoint.Y, GetActorLocation().Z) - GetActorLocation()).GetSafeNormal();	// We couldn't do "hitVector = (hitPoint - GetActorLocation()).GetSafeNormal(); hitVector.Z = actorForwardVector.Z;" because of the normalization. We should do instead "hitVector = (hitPoint - GetActorLocation()); hitVector.Z = actorForwardVector.Z; hitVector = hitVector.GetSafeNormal();", which is equivalent to this line here

	// dotProd(afv, hv) = |afv||hv| * cos(theta) ; but |afv| = |hv| = 1 since they are normalized vectors, so dotProd(afv, hv) = cos(theta)
	float cosTheta = FVector::DotProduct(actorForwardVector, hitVector);
	float angleInRadians = acos(cosTheta);
	float angleInDegrees = FMath::RadiansToDegrees(angleInRadians);

	/* DEBUG */
	/*DRAW_SPHERE(hitPoint, 10.f);
	UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(), GetActorLocation() + 100.f * actorForwardVector, 5.f, FColor::Cyan, 10.f);
	UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(), GetActorLocation() + 100.f * hitVector, 5.f, FColor::Green, 10.f);

	UE_LOG(LogTemp, Warning, TEXT("degrees %f"), angleInDegrees);
	UE_LOG(LogTemp, Warning, TEXT("degrees %f"), FVector::CrossProduct(actorForwardVector, hitVector).Z);*/
	/* END DEBUG */

	if (0.f < angleInDegrees && angleInDegrees < frontBackAngle)
		return FName("HitFromFront");
	else if ((180.f - frontBackAngle) < angleInDegrees && angleInDegrees < 180.f)
		return FName("HitFromBack");
	else
	{
		// The crossProd for 2 vectors in the same plane is a perpendicular one (positive or negative). Components X & Y are 0, and in Z it is the magnitude of the resulting product
		float fromLeftOrRight = FVector::CrossProduct(actorForwardVector, hitVector).Z;
		if (fromLeftOrRight < 0.f)
			return FName("HitFromLeft");
		else
			return FName("HitFromRight");
	}
}