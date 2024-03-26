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

void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	if (healthBar)
	{
		healthBar->setPercentage(1.f);
		healthBar->SetVisibility(false);
	}

	// AI
	aiController = Cast<AAIController>(GetController());
	if (currentPatrolTarget)
		moveToTarget(currentPatrolTarget);

	if (sensingComponent)
		sensingComponent->OnSeePawn.AddDynamic(this, &AEnemy::seenPawn);

	// Death
	if (materialInstanceDynamic)
		ditheringMaterial = UMaterialInstanceDynamic::Create(materialInstanceDynamic, this);

	if (deathPetals)
		deathPetals->Deactivate();

	// Weapon
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
	if (attributes->isAlive())
	{
		if (healthBar)
			healthBar->SetVisibility(true);

		FName directionName = getHitDirection(hitPoint);
		playMontage(hitMontage, directionName);
	}
	else
	{
		int8 playedDeath_Index = playDeathMontage();

		if (livingState == ELivingState::ELS_Alive)
		{
			die();
			fadeOut();
		}

		livingState = getDeathType(playedDeath_Index);

		deathIndex = deathAnimationAfterDead_index;
	}

	// Play sfx and vfx in any case
	if (hitSound)
		UGameplayStatics::PlaySoundAtLocation(this, hitSound, GetActorLocation());

	if (hitVFX)
		UGameplayStatics::SpawnEmitterAtLocation(this, hitVFX, hitPoint);
}

ELivingState AEnemy::getDeathType(int8 inDeathIndex)
{
	switch (inDeathIndex)
	{
	case(0):
		return ELivingState::ELS_Dead1;
	case(1):
		return ELivingState::ELS_Dead2;
	case(2):
		return ELivingState::ELS_Dead3;
	default:
		return ELivingState::ELS_Dead1;
	}
}

void AEnemy::die()
{
	healthBar->SetVisibility(false);

	// Stop doing whatever it is doing
	GetWorldTimerManager().ClearTimer(patrolTimer);

	if (aiController)
	{
		aiController->StopMovement();
		aiController->ClearFocus(EAIFocusPriority::Gameplay);
	}

	SetActorTickEnabled(false);

	// Disable all collisions
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
}

void AEnemy::fadeOut()
{
	if (GetMesh()) GetMesh()->SetMaterial(0, ditheringMaterial);
	if (ditheringMaterial) ditheringMaterial->SetScalarParameterValue("totalDitheringTime", dithering_totalTime);
	
	dithering_startTime = GetWorld()->GetTimeSeconds();

	GetWorldTimerManager().SetTimer<AEnemy>(timerHandler_dithering, this, &AEnemy::decreaseDitheringOnMaterial, dithering_execRate, true, dithering_initialDelay);
	GetWorldTimerManager().SetTimer<AEnemy>(timerHandler_deathPetals, this, &AEnemy::activateDeathPetalsAnim, deathPetals_initialDelay);

	SetLifeSpan(dithering_initialDelay + dithering_totalTime + dithering_extraTime);
}

void AEnemy::decreaseDitheringOnMaterial()
{
	if (ditheringMaterial)
	{
		float ditheringCurrentTime = dithering_startTime + dithering_initialDelay;
		ditheringMaterial->SetScalarParameterValue("elapsedTime", GetWorld()->GetTimeSeconds() - ditheringCurrentTime);
	}
}

void AEnemy::activateDeathPetalsAnim()
{
	if (deathPetals)
		deathPetals->Activate(true);
}

/*
* Take damage
*/

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (!attributes) return -1.f;		// If health cannot be updated, then we shouldn't process damage at all (?)

	attributes->takeDamage(DamageAmount);
	if (healthBar)
		healthBar->setPercentage(attributes->getHealthPercent());

	bool isStillAlive = attributes->isAlive();
	if (isStillAlive)					// Start chasing in case the Character attacks from behind (and Enemy doesn't see it). Combine this with OnHearNoise and it could be a nice stealth mechanic
	{
		state = EEnemyState::EES_Chasing;
		combatTarget = EventInstigator->GetPawn();

		GetCharacterMovement()->MaxWalkSpeed = attributes->getChasingSpeed();
		setFocalPointToActor(combatTarget);
		moveToTarget(combatTarget, attackRadius - 110.f);
	}

	return DamageAmount;
}

void AEnemy::setFocalPointToActor(AActor* actor)
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

void AEnemy::resetFocalPoint()
{
	setFocalPointToActor(nullptr);
}

/* 
* Tick and general methods
*/

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	showOrHideHealthBar();

	if (state != EEnemyState::EES_Patrolling)	// Pawn seen!
		combat();
	else												// Pawn out of the combat range
		patrol();
}

void AEnemy::showOrHideHealthBar()
{
	bool showHealthBar = isActorWithinRadius(combatTarget, combatRadius);
	if (healthBar)
		healthBar->SetVisibility(showHealthBar);
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

/*
* Combat
*/

void AEnemy::seenPawn(APawn* pawn)
{
	if (state != EEnemyState::EES_Patrolling) return;

	if (pawn->ActorHasTag(FName("SlashCharacter")))		// Casting is very expensive to be performed multiple times (ASlashCharacter* character = Cast<ASlashCharacter>(pawn)), so we use tags instead to check to pawn type!
	{
		bool shouldChaseCharacter = isActorWithinRadius(pawn, combatRadius);
		if (shouldChaseCharacter && state == EEnemyState::EES_Patrolling)
		{
			state = EEnemyState::EES_Chasing;

			GetCharacterMovement()->MaxWalkSpeed = attributes->getChasingSpeed();

			GetWorldTimerManager().ClearTimer(patrolTimer);

			combatTarget = pawn;
			moveToTarget(combatTarget, attackRadius - 110.f);
		}
	}
}

void AEnemy::combat()
{
	// This system of chasing/attacking/interest-loosing is designed as 2 concentric circles, defining attackRadius the smallast one, and combatRadius the biggest one.
	bool isActorInRange = isActorWithinRadius(combatTarget, combatRadius);
	if (!isActorInRange)																					// Outside the combatRadius -> should get back to patrol
	{
		state = EEnemyState::EES_Patrolling;

		combatTarget = nullptr;

		if (healthBar)
			healthBar->SetVisibility(false);

		GetCharacterMovement()->MaxWalkSpeed = attributes->getPatrollingSpeed();
		moveToTarget(currentPatrolTarget);
	}
	else if (!isActorWithinRadius(combatTarget, attackRadius) && state != EEnemyState::EES_Chasing)			// Inside combatRadius, outside attackRadius- > should keep chasing
	{
		state = EEnemyState::EES_Chasing;
		
		resetFocalPoint();

		GetCharacterMovement()->MaxWalkSpeed = attributes->getChasingSpeed();
		moveToTarget(combatTarget, attackRadius - 110.f);
	}
	else if (isActorWithinRadius(combatTarget, attackRadius) && state != EEnemyState::EES_Attacking)		// [inside combatRadius and] Inside attackRadius -> should attack
	{
		state = EEnemyState::EES_Attacking;

		setFocalPointToActor(combatTarget);


	}
}

/*
* Patrol
*/

void AEnemy::patrol()
{
	// Even if we already have these checks within selectNextPatrolTarget() and moveToTarget() methods, check them first of all in order to not waste
	// computing time if we just can't patrol. Good practice overall. E.g. selectNextPatrolTarget() method could be really expensive
	if (patrolPoints.Num() <= 0 || aiController == nullptr) return;

	bool hasReachedTarget = isActorWithinRadius(currentPatrolTarget, patrolRadius);
	if (hasReachedTarget)
	{
		AActor* nextTarget = selectNextPatrolTarget();
		currentPatrolTarget = nextTarget;

		GetWorldTimerManager().SetTimer<AEnemy>(patrolTimer, this, &AEnemy::finishedTimer, 5.f);
	}
}

AActor* AEnemy::selectNextPatrolTarget()
{
	TArray<AActor*> remainingPatrolPoints;
	for (AActor* actor : patrolPoints)
	{
		if (actor != currentPatrolTarget)
			remainingPatrolPoints.Add(actor);
	}

	if (remainingPatrolPoints.Num() > 0)
	{
		const int16 randomTargetIndex = FMath::RandRange(0, remainingPatrolPoints.Num() - 1);
		return remainingPatrolPoints[randomTargetIndex];
	}

	return nullptr;

	//// This code can be used for future enhancement (if an enemy lose a target for whatever reason, it should be moved to its closest patrol point first)
	//int16 closestPatrolPoint = 0;
	//float closestPatrolPoint_dist = std::numeric_limits<float>::max();
	//for (int i = 0; i < validPatrolPoints.Num(); i++)
	//{
	//	float distanceToPatrolPoint = (validPatrolPoints[i]->GetActorLocation() - GetActorLocation()).Size();
	//	if (distanceToPatrolPoint < closestPatrolPoint_dist)
	//	{
	//		closestPatrolPoint = i;
	//		closestPatrolPoint_dist = distanceToPatrolPoint;
	//	}
	//}

	// return validPatrolPoints[closestPatrolPoint];
}

void AEnemy::moveToTarget(const AActor* target, float acceptanceRadius)
{
	if (aiController == nullptr || target == nullptr) return;

	FAIMoveRequest moveRequest;
	moveRequest.SetGoalActor(target);
	moveRequest.SetAcceptanceRadius(acceptanceRadius);
	FNavPathSharedPtr navPath;
	aiController->MoveTo(moveRequest, &navPath);
}

void AEnemy::finishedTimer()
{
	moveToTarget(currentPatrolTarget);
}

