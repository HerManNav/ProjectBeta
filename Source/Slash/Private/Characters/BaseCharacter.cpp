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

	if (Attributes)
		RecoverHealth(DeltaTime * Attributes->GetHealthRecoveryRate());
}

void ABaseCharacter::RecoverHealth(float RecoverAmount)
{
	if (Attributes && !Attributes->IsHealthMaxedOut())
	{
		Attributes->RecoverHealth(RecoverAmount);
		UpdateHealthBar();
	}
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
* Hit / Take Damage 
*/

void ABaseCharacter::GetHit_Implementation(const FVector& HitPoint)
{
	if (HasSomeHealthRemaining())
		ReactToHitBasedOnHitDirection(HitPoint);
	else Die();

	SetWeaponCollision(ECollisionEnabled::NoCollision);

	PlayHitSoundAtLocation(HitPoint);
	PlayHitParticlesAtLocation(HitPoint);
}

bool ABaseCharacter::HasSomeHealthRemaining()
{
	return Attributes? Attributes->IsAlive() : false;
}

void ABaseCharacter::Die_Implementation()
{
}

float ABaseCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (!CanTakeDamage()) return -1.f;

	ActuallyReceiveDamage(DamageAmount);
	UpdateHealthBar();

	return DamageAmount;
}

bool ABaseCharacter::CanTakeDamage()
{
	return Attributes != nullptr;
}

void ABaseCharacter::ActuallyReceiveDamage(float DamageAmount)
{
	if (Attributes)
		Attributes->ReceiveDamage(DamageAmount);
}

void ABaseCharacter::DisableCollisionsToDie()
{
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
}

void ABaseCharacter::DisableAttributesRegen()
{
	Attributes = nullptr;
}

/*
* Montages
*/

int16 ABaseCharacter::GetAttackMontageNumberOfSections()
{
	if (AttackMontage) return AttackMontage->GetNumSections();
	return 0;
}

int16 ABaseCharacter::GetDeathMontageNumberOfSections()
{
	if (DeathMontage) return DeathMontage->GetNumSections();
	return 0;
}

int16 ABaseCharacter::PlayAttackingMontage()
{
	return PlayRandomMontageSection(AttackMontage, AttackMontageSectionNames, AttackIndex);
}

int16 ABaseCharacter::PlayDeathMontage()
{
	return PlayRandomMontageSection(DeathMontage, DeathMontageSectionNames, DeathIndex);
}

void ABaseCharacter::PlayMontage(UAnimMontage* Montage, FName SectionName)
{
	TObjectPtr<UAnimInstance> AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && Montage)
	{
		AnimInstance->Montage_Play(Montage, 1.f);
		AnimInstance->Montage_JumpToSection(SectionName, Montage);
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
	if (HitReactMontage)
	{
		FName DirectionName = GetHitDirection(HitPoint);
		PlayMontage(HitReactMontage, DirectionName);
	}
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
	/*DRAW_SPHERE(HitPoint, 10.f);
	UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(), GetActorLocation() + 100.f * ActorForwardVector, 5.f, FColor::Cyan, 10.f);
	UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(), GetActorLocation() + 100.f * HitVector, 5.f, FColor::Green, 10.f);

	UE_LOG(LogTemp, Warning, TEXT("AngleInDegrees %f"), AngleInDegrees);
	UE_LOG(LogTemp, Warning, TEXT("FVector::CrossProduct(ActorForwardVector, HitVector).Z %f"), FVector::CrossProduct(ActorForwardVector, HitVector).Z);*/
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

FVector ABaseCharacter::GetRotationTargetForMotionWarping()
{
	if (CombatTarget) return CombatTarget->GetActorLocation();
	return FVector::ZeroVector;
}

FVector ABaseCharacter::GetLocationTargetForMotionWarping()
{
	if (!CombatTarget) return FVector::ZeroVector;
	 
	FVector CombatTargetLocation = CombatTarget->GetActorLocation();
	FVector TargetToMe = (GetActorLocation() - CombatTargetLocation).GetSafeNormal();

	return CombatTargetLocation + DistanceToTarget_MotionWarping * TargetToMe;
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