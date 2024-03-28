// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/Enemy.h"
#include "Characters/SlashCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Slash/DebugMacros.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Math/UnrealMathUtility.h"
#include "Components/AttributesComponent.h"
#include "HUD/HealthBarComponent.h"
#include "Items/Weapons/Weapon.h"

#include "Materials/MaterialInstanceDynamic.h"
#include "NiagaraComponent.h"

#include "AIController.h"
#include "Perception/PawnSensingComponent.h"

AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	healthBar = CreateDefaultSubobject<UHealthBarComponent>(TEXT("healthBar"));
	healthBar->SetupAttachment(RootComponent);

	deathPetals = CreateDefaultSubobject<UNiagaraComponent>(TEXT("niagara_deathPetals"));
	deathPetals->SetupAttachment(GetMesh(), TEXT("Hips"));

	sensingComponent = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("sensingComponent"));
	sensingComponent->SightRadius = 3000.f;
	sensingComponent->SetPeripheralVisionAngle(60.f);

	GetMesh()->SetGenerateOverlapEvents(true);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	GetCharacterMovement()->MaxWalkSpeed = attributes->getPatrollingSpeed();
	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
}

/*
* Init methods 
*/
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	InitHealthBar();
	InitAI();
	InitDeathMembers();
	EquipWeapon();
}

void AEnemy::InitHealthBar()
{
	if (healthBar) healthBar->setPercentage(1.f);
	
	HideHealthBar();
}

void AEnemy::InitAI()
{
	aiController = Cast<AAIController>(GetController());
	if (currentPatrolTarget)
		moveToTarget(currentPatrolTarget);

	if (sensingComponent)
		sensingComponent->OnSeePawn.AddDynamic(this, &AEnemy::PawnSeen);
}

void AEnemy::InitDeathMembers()
{
	if (materialInstanceDynamic)
		ditheringMaterial = UMaterialInstanceDynamic::Create(materialInstanceDynamic, this);

	if (deathPetals)
		deathPetals->Deactivate();
}

void AEnemy::EquipWeapon()
{
	if (GetWorld() && weaponClass)
	{
		weapon = GetWorld()->SpawnActor<AWeapon>(weaponClass);
		if (weapon && GetMesh())
			weapon->Equip(GetMesh(), FName("RightHandSocket"), this, this);
	}
}

void AEnemy::Destroyed()
{
	if (weapon)
		weapon->Destroy();
}


/*
* GetHit 
*/

void AEnemy::getHit_Implementation(const FVector& hitPoint)
{
	if (IsAlive())
	{
		if (IsEngaged()) AttackEnd();

		ShowHealthBar();
		PlayHitAnimationBasedOnHitPoint(hitPoint);
	}
	else Die();

	PlayHitSoundAtLocation(hitPoint);
	PlayHitParticlesAtLocation(hitPoint);
}

void AEnemy::PlayHitAnimationBasedOnHitPoint(const FVector& hitPoint)
{
	FName directionName = getHitDirection(hitPoint);
	playMontage(hitMontage, directionName);
}

void AEnemy::PlayHitSoundAtLocation(const FVector& location)
{
	if (hitSound) UGameplayStatics::PlaySoundAtLocation(this, hitSound, location);
}

void AEnemy::PlayHitParticlesAtLocation(const FVector& location)
{
	if (hitVFX) UGameplayStatics::SpawnEmitterAtLocation(this, hitVFX, location);
}


/*
* Die
*/

void AEnemy::Die()
{
	HideHealthBar();

	GetWorldTimerManager().ClearAllTimersForObject(this);

	StopAIController();
	SetActorTickEnabled(false);

	DisableCollisionsForPawn();

	PlayDeathAnimation();
	FadeOut();
}

void AEnemy::StopAIController()
{
	if (aiController)
	{
		aiController->StopMovement();
		aiController->ClearFocus(EAIFocusPriority::Gameplay);
	}
}

void AEnemy::DisableCollisionsForPawn()
{
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
}

void AEnemy::PlayDeathAnimation()
{
	int8 playedDeath_Index = playDeathMontage();
	DeathPose = GetDeathPose(playedDeath_Index);
}

EDeathPose AEnemy::GetDeathPose(int8 inDeathIndex)
{
	switch (inDeathIndex)
	{
	case(0):
		return EDeathPose::EDP_Dead1;
	case(1):
		return EDeathPose::EDP_Dead2;
	case(2):
		return EDeathPose::EDP_Dead3;
	default:
		return EDeathPose::EDP_Dead1;
	}
}

void AEnemy::FadeOut()
{
	if (GetMesh()) GetMesh()->SetMaterial(0, ditheringMaterial);
	if (ditheringMaterial) ditheringMaterial->SetScalarParameterValue("totalDitheringTime", dithering_totalTime);
	
	dithering_startTime = GetWorld()->GetTimeSeconds();

	GetWorldTimerManager().SetTimer<AEnemy>(timerHandler_dithering, this, &AEnemy::DecreaseDitheringOnMaterial, dithering_execRate, true, dithering_initialDelay);
	GetWorldTimerManager().SetTimer<AEnemy>(timerHandler_deathPetals, this, &AEnemy::ActivateDeathPetalsAnim, deathPetals_initialDelay);

	SetLifeSpan(dithering_initialDelay + dithering_totalTime + dithering_extraTime);
}

void AEnemy::DecreaseDitheringOnMaterial()
{
	if (ditheringMaterial)
	{
		float ditheringCurrentTime = dithering_startTime + dithering_initialDelay;
		ditheringMaterial->SetScalarParameterValue("elapsedTime", GetWorld()->GetTimeSeconds() - ditheringCurrentTime);
	}
}

void AEnemy::ActivateDeathPetalsAnim()
{
	if (deathPetals)
		deathPetals->Activate(true);
}


/*
* Take damage
*/

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (!CanTakeDamage()) return -1.f;

	ActuallyReceiveDamage(DamageAmount);
	UpdateHealthBar();
	
	if (IsAlive() && !IsAwareOfCharacter())
	{
		//SetFocalPointToActor(combatTarget);

		ClearPatrolTimer();
		SetCombatTarget(EventInstigator->GetPawn());
		ChaseCurrentTarget();
	}

	return DamageAmount;
}

bool AEnemy::CanTakeDamage()
{
	return attributes && healthBar;
}

void AEnemy::ActuallyReceiveDamage(float DamageAmount)
{
	attributes->takeDamage(DamageAmount);
	if (!attributes->isAlive()) EnemyState = EEnemyState::EES_Dead;
}

void AEnemy::UpdateHealthBar()
{
	if (healthBar)
		healthBar->setPercentage(attributes->getHealthPercent());
}

bool AEnemy::IsAwareOfCharacter()
{
	return EnemyState > EEnemyState::EES_Patrolling;
}


/* 
* Tick
*/

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsDead()) return;

	if (!IsPatrolling())		// Pawn seen!
		CheckCombat();
	else						// Pawn out of the combat range
		CheckPatrol();
}


/*
* Combat: Pawn seen
*/

void AEnemy::PawnSeen(APawn* pawn)
{
	if (!IsPatrolling() || IsDead()) return;

	if (IsPawnMainCharacter(pawn))
	{
		if (IsCharacterInsideCombatRange(pawn) && IsPatrolling())
		{
			ShowHealthBar();

			ClearPatrolTimer();
			SetCombatTarget(pawn);

			ChaseCurrentTarget();
		}
	}
}

bool AEnemy::IsPawnMainCharacter(APawn* pawn)
{
	return pawn->ActorHasTag(FName("SlashCharacter"));		// Casting is very expensive to be performed multiple times (ASlashCharacter* character = Cast<ASlashCharacter>(pawn)), so we use tags instead to check to pawn type!
}

bool AEnemy::IsCharacterInsideCombatRange(APawn* pawn)
{
	return isActorWithinRadius(pawn, combatRadius);
}

void AEnemy::ClearPatrolTimer()
{
	GetWorldTimerManager().ClearTimer(patrolTimer);
}


/*
* Combat: main logic
*/

void AEnemy::CheckCombat()
{
	// This system of chasing/attacking/interest-loosing is designed as 2 concentric circles, defining attackRadius the smallast one, and combatRadius the biggest one.

	if (IsCharacterOutOfRange() && !IsEngaged())
	{
		ClearAttackTimer();
		HideHealthBar();

		LoseInterest();
	}
	else if (ShouldChaseCurrentTarget())
	{
		ClearAttackTimer();
		ResetFocalPoint();

		ChaseCurrentTarget();
	}
	else if (CanAttack())
	{
		ClearAttackTimer();
		SetFocalPointToActor(combatTarget);

		SetAttackTimer();
	}
}

bool AEnemy::IsCharacterOutOfRange()
{
	return !isActorWithinRadius(combatTarget, combatRadius);
}

bool AEnemy::IsCharacterOutOfAttackRange()
{
	return !IsCharacterInsideAttackRange();
}

bool AEnemy::IsCharacterInsideAttackRange()
{
	return isActorWithinRadius(combatTarget, attackRadius);
}

void AEnemy::ClearAttackTimer()
{
	GetWorldTimerManager().ClearTimer(AttackTimer);
}

void AEnemy::LoseInterest()
{
	combatTarget = nullptr;
	ResetFocalPoint();

	EnemyState = EEnemyState::EES_Patrolling;
	GetCharacterMovement()->MaxWalkSpeed = attributes->getPatrollingSpeed();
	moveToTarget(currentPatrolTarget);
}

void AEnemy::ChaseCurrentTarget()
{
	EnemyState = EEnemyState::EES_Chasing;
	GetCharacterMovement()->MaxWalkSpeed = attributes->getChasingSpeed();
	moveToTarget(combatTarget, attackRadius - 110.f);
}

void AEnemy::SetAttackTimer()
{
	EnemyState = EEnemyState::EES_Attacking;

	const float AttackTime = FMath::RandRange(MinAttackWait, MaxAttackWait);
	GetWorldTimerManager().SetTimer(AttackTimer, this, &AEnemy::Attack, AttackTime);
}

void AEnemy::Attack()
{
	Super::Attack();

	EnemyState = EEnemyState::EES_Engaged;
	PlayAttackingMontage();
	ResetFocalPoint();
}

void AEnemy::AttackEnd()
{
	EnemyState = EEnemyState::EES_NoState;
	CheckCombat();
}

/*
* Patrol: main logic
*/

void AEnemy::CheckPatrol()
{
	if (!CanPatrol()) return;

	if (HasReachedCurrentTarget())
	{
		currentPatrolTarget = selectNextPatrolTarget();
		MoveToNextPatrolTargetAfterSeconds(5.f);
	}
}

bool AEnemy::CanPatrol()
{
	return patrolPoints.Num() > 0 && aiController != nullptr;
}

bool AEnemy::HasReachedCurrentTarget()
{
	return isActorWithinRadius(currentPatrolTarget, patrolRadius);
}

AActor* AEnemy::selectNextPatrolTarget()
{
	TArray<AActor*> remainingPatrolPoints;

	GetRemainingPatrolPoints(remainingPatrolPoints);
	return GetRandomActorFromArray(remainingPatrolPoints);
}

void AEnemy::GetRemainingPatrolPoints(TArray<AActor*>& remainingPatrolPoints)
{
	for (AActor* actor : patrolPoints)
	{
		if (actor != currentPatrolTarget)
			remainingPatrolPoints.Add(actor);
	}
}

AActor* AEnemy::GetRandomActorFromArray(const TArray<AActor*>& remainingPatrolPoints)
{
	if (remainingPatrolPoints.Num() > 0)
	{
		const int16 randomTargetIndex = FMath::RandRange(0, remainingPatrolPoints.Num() - 1);
		return remainingPatrolPoints[randomTargetIndex];
	}

	return nullptr;
}

void AEnemy::MoveToNextPatrolTargetAfterSeconds(float seconds)
{
	GetWorldTimerManager().SetTimer<AEnemy>(patrolTimer, this, &AEnemy::MoveToPatrolTarget, seconds);
}


/*
* General common methods
*/

void AEnemy::SetCombatTarget(AActor* target)
{
	combatTarget = target;
}

void AEnemy::SetFocalPointToActor(AActor* actor)
{
	if (!aiController) return;

	if (actor)
	{
		GetCharacterMovement()->bOrientRotationToMovement = false;
		GetCharacterMovement()->bUseControllerDesiredRotation = true;
		aiController->SetFocus(actor);
	}
	else
	{
		GetCharacterMovement()->bOrientRotationToMovement = true;
		GetCharacterMovement()->bUseControllerDesiredRotation = false;
		aiController->ClearFocus(EAIFocusPriority::Gameplay);
	}
}

void AEnemy::ResetFocalPoint()
{
	SetFocalPointToActor(nullptr);
}

bool AEnemy::isActorWithinRadius(AActor* actor, float radius)
{
	if (actor == nullptr) return false;

	FVector actorLocation = actor->GetActorLocation();
	FVector selfLocation = GetActorLocation();

	if (FMath::IsNearlyEqual(actorLocation.X, selfLocation.X) &&
		FMath::IsNearlyEqual(actorLocation.Y, selfLocation.Y) &&
		FMath::IsNearlyEqual(actorLocation.Z, selfLocation.Z))
		return true;

	return (actor->GetActorLocation() - GetActorLocation()).Size() <= radius;
}

void AEnemy::moveToTarget(const AActor* target, float acceptanceRadius)
{
	if (!CanMoveToTarget(target)) return;

	FAIMoveRequest moveRequest;
	moveRequest.SetGoalActor(target);
	moveRequest.SetAcceptanceRadius(acceptanceRadius);
	aiController->MoveTo(moveRequest);
}

bool AEnemy::CanMoveToTarget(const AActor* target)
{
	return target != nullptr && aiController != nullptr;
}

void AEnemy::MoveToPatrolTarget()
{
	moveToTarget(currentPatrolTarget);
}

bool AEnemy::ShouldChaseCurrentTarget()
{
	return IsCharacterOutOfAttackRange() && !IsChasing() && !IsEngaged();
}

bool AEnemy::CanAttack()
{
	return IsCharacterInsideAttackRange() && !IsDead() && !IsAttacking() && !IsEngaged();
}

bool AEnemy::IsDead()
{
	return EnemyState == EEnemyState::EES_Dead;
}

bool AEnemy::IsAlive()
{
	return EnemyState > EEnemyState::EES_Dead;
}

bool AEnemy::IsPatrolling()
{
	return EnemyState == EEnemyState::EES_Patrolling;
}

bool AEnemy::IsChasing()
{
	return EnemyState == EEnemyState::EES_Chasing;
}

bool AEnemy::IsAttacking()
{
	return EnemyState == EEnemyState::EES_Attacking;
}

bool AEnemy::IsEngaged()
{
	return EnemyState == EEnemyState::EES_Engaged;
}

void AEnemy::ShowHealthBar()
{
	if (healthBar) healthBar->SetVisibility(true);
}

void AEnemy::HideHealthBar()
{
	if (healthBar) healthBar->SetVisibility(false);
}