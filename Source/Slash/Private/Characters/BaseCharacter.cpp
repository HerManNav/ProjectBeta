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

	/*if (!attackMontage || 
		(- 1 > attackIndex || attackIndex > (attackMontage->GetNumSections() - 1)))
	{
		attackIndex = -1;

		UE_LOG(LogTemp, Warning, TEXT("attackMontage not set or attackIndex out of attackMontage bounds. attackIndex reset to -1."));
	}

	if (!deathMontage || 
		(- 1 > deathIndex || deathIndex > (deathMontage->GetNumSections() - 1)))
	{
		deathIndex = -1;

		UE_LOG(LogTemp, Warning, TEXT("deathMontage not set or deathIndex out of deathMontage bounds. deathMontage reset to -1."));
	}*/
}

void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

/*
* Attack
*/

void ABaseCharacter::Attack()
{
	if (CanAttack())
	{
		playAttackingMontage();
		actionState = EActionState::EAS_Attacking;
	}
}

void ABaseCharacter::attackEnd()
{
	actionState = EActionState::EAS_Unoccupied;
}

void ABaseCharacter::playAttackingMontage()
{
	TObjectPtr<UAnimInstance> animInstance = GetMesh()->GetAnimInstance();

	if (attackMontage)
	{
		int8 selectedAttack_Index = attackIndex;
		if (attackIndex == -1)
			selectedAttack_Index = FMath::RandRange(0, attackMontage->GetNumSections() - 1);

		playMontage(attackMontage, attackMontage->GetSectionName(selectedAttack_Index));
	}
}

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

/*
* Death
*/

void ABaseCharacter::playMontage(UAnimMontage* montage, FName montageName)
{
	TObjectPtr<UAnimInstance> animInstance = GetMesh()->GetAnimInstance();
	if (animInstance && montage)
	{
		animInstance->Montage_Play(montage, 1.f);
		animInstance->Montage_JumpToSection(montageName, montage);
	}
}

int8 ABaseCharacter::playDeathMontage()
{
	if (deathMontage)
	{
		if (deathIndex == -1)
			deathIndex = FMath::RandRange(0, deathMontage->GetNumSections() - 1);

		playMontage(deathMontage, deathMontage->GetSectionName(deathIndex));

		return deathIndex;
	}
	return -1;
}

