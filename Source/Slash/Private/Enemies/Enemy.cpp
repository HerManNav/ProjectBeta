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

	// Death
	if (materialInstanceDynamic)
		dynamicMaterial = UMaterialInstanceDynamic::Create(materialInstanceDynamic, this);

	if (deathPetals)
		deathPetals->Deactivate();

	// AI
	aiController = Cast<AAIController>(GetController());
	if (currentPatrolTarget)
		moveToTarget(currentPatrolTarget);

	if (sensingComponent)
		sensingComponent->OnSeePawn.AddDynamic(this, &AEnemy::seenPawn);
}

/*
* GetHit 
*/

void AEnemy::getHit_Implementation(const FVector& hitPoint)
{
	if (healthBar)
		healthBar->SetVisibility(true);

	if (attributes->isAlive())
	{
		FName directionName = getHitDirection(hitPoint);
		playMontage(hitMontage, directionName);
	}
	else
	{
		playDeathMontage();

		die();
		fadeOut();
	}

	// Play sfx and vfx in any case
	if (hitSound)
		UGameplayStatics::PlaySoundAtLocation(this, hitSound, GetActorLocation());

	if (hitVFX)
		UGameplayStatics::SpawnEmitterAtLocation(this, hitVFX, hitPoint);
}

void AEnemy::playDeathMontage()
{
	if (deathMontage)
	{
		if (deathIndex == -1)
			deathIndex = FMath::RandRange(0, deathMontage->GetNumSections() - 1);

		playMontage(deathMontage, deathMontage->GetSectionName(deathIndex));

		switch (deathIndex)
		{
		case(0):
			livingState = ELivingState::ELS_Dead1;
			break;
		case(1):
			livingState = ELivingState::ELS_Dead2;
			break;
		case(2):
			livingState = ELivingState::ELS_Dead3;
			break;
		default:
			break;
		}
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
	// BUG: this method crashes sometimes. Ideas: the lambda functions might need to get extracted in order to be class methods, just like the handlers themselves (using local variables is not a good idea for timers?)
	float dithering_initial = dithering;

	auto decreaseDitheringOnMaterial = [&]()
		{
			if (dynamicMaterial)
			{
				dynamicMaterial->SetScalarParameterValue("dithering", dithering -= dithering_fadeOutRate);
				if (GetMesh()) GetMesh()->SetMaterial(0, dynamicMaterial);
			}
		};

	auto activateDeathPetals = [&]()
		{
			if (deathPetals)
				deathPetals->Activate(true);
		};

	FTimerHandle timerHandler_dithering;
	GetWorldTimerManager().SetTimer(timerHandler_dithering, decreaseDitheringOnMaterial, dithering_execRate, true, dithering_delay);

	FTimerHandle timerHandler_deathPetals;
	GetWorldTimerManager().SetTimer(timerHandler_deathPetals, activateDeathPetals, deathPetals_delay, false);

	SetLifeSpan(dithering_delay + dithering_initial / dithering_fadeOutRate * dithering_execRate + dithering_extraTime);
	//GetWorldTimerManager().ClearTimer(timerHandler);		// Not needed since at the end of the lifespan this actor is destroyed? And so it is its timers.
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
		combatState = ECombatState::ECS_Chasing;
		combatTarget = EventInstigator->GetPawn();

		GetCharacterMovement()->MaxWalkSpeed = attributes->getChasingSpeed();
		setFocalPointToActor(combatTarget);
		moveToTarget(combatTarget, attackRadius - 110.f);
	}

	return DamageAmount;
}

void AEnemy::setFocalPointToActor(AActor* actor)
{
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

	if (combatState != ECombatState::ECS_Patrolling)	// Pawn seen!
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
	if (combatState != ECombatState::ECS_Patrolling) return;

	if (pawn->ActorHasTag(FName("SlashCharacter")))		// Casting is very expensive to be performed multiple times (ASlashCharacter* character = Cast<ASlashCharacter>(pawn)), so we use tags instead to check to pawn type!
	{
		bool shouldChaseCharacter = isActorWithinRadius(pawn, combatRadius);
		if (shouldChaseCharacter && combatState == ECombatState::ECS_Patrolling)
		{
			combatState = ECombatState::ECS_Chasing;

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
	if (!isActorInRange)																						// Outside the combatRadius -> should get back to patrol
	{
		combatState = ECombatState::ECS_Patrolling;

		combatTarget = nullptr;

		if (healthBar)
			healthBar->SetVisibility(false);

		GetCharacterMovement()->MaxWalkSpeed = attributes->getPatrollingSpeed();
		moveToTarget(currentPatrolTarget);
	}
	else if (!isActorWithinRadius(combatTarget, attackRadius) && combatState != ECombatState::ECS_Chasing)		// Inside combatRadius, outside attackRadius- > should keep chasing
	{
		combatState = ECombatState::ECS_Chasing;
		
		resetFocalPoint();

		GetCharacterMovement()->MaxWalkSpeed = attributes->getChasingSpeed();
		moveToTarget(combatTarget, attackRadius - 110.f);
	}
	else if (isActorWithinRadius(combatTarget, attackRadius) && combatState != ECombatState::ECS_Attacking)		// [inside combatRadius and] Inside attackRadius -> should attack
	{
		combatState = ECombatState::ECS_Attacking;

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

