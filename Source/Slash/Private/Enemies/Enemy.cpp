// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/Enemy.h"
#include "Items/Weapons/Weapon.h"
#include "Items/Pickups/Soul.h"
#include "HUD/HealthBarComponent.h"
#include "Components/AttributesComponent.h"

#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "Materials/MaterialInstanceDynamic.h"
#include "NiagaraComponent.h"

#include "AIController.h"
#include "Perception/PawnSensingComponent.h"

AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	HealthBar = CreateDefaultSubobject<UHealthBarComponent>(TEXT("HealthBar"));
	HealthBar->SetupAttachment(RootComponent);

	LockOnWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("LockOnWidget"));
	LockOnWidget->SetupAttachment(RootComponent);

	DeathPetals = CreateDefaultSubobject<UNiagaraComponent>(TEXT("niagara_deathPetals"));
	DeathPetals->SetupAttachment(GetMesh(), TEXT("Hips"));

	SensingComponent = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("SensingComponent"));
	SensingComponent->SightRadius = 3000.f;
	SensingComponent->SetPeripheralVisionAngle(60.f);

	GetMesh()->SetGenerateOverlapEvents(true);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	GetCharacterMovement()->MaxWalkSpeed = GetPatrollingSpeed();
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

	Tags.Add(FName("Enemy"));

	InitHealthBar();
	InitAI();
	InitDeathMembers();
	SpawnAndEquipWeapon();
}

void AEnemy::InitHealthBar()
{
	if (HealthBar) HealthBar->SetPercentage(1.f);
	
	HideHealthBar();
}

void AEnemy::InitAI()
{
	AIController = Cast<AAIController>(GetController());
	if (CurrentPatrolTarget)
		MoveToTarget(CurrentPatrolTarget);

	if (SensingComponent)
		SensingComponent->OnSeePawn.AddDynamic(this, &AEnemy::PawnSeen);
}

void AEnemy::InitDeathMembers()
{
	if (AfterDeathDitheringMI)
		DitheringMaterial = UMaterialInstanceDynamic::Create(AfterDeathDitheringMI, this);

	if (DeathPetals)
		DeathPetals->Deactivate();
}

void AEnemy::SpawnAndEquipWeapon()
{
	if (GetWorld() && WeaponClass)
	{
		Weapon = GetWorld()->SpawnActor<AWeapon>(WeaponClass);
		if (Weapon && GetMesh())
			Weapon->Equip(GetMesh(), FName("WeaponSocket"), this, this);
	}
}

void AEnemy::Destroyed()
{
	if (Weapon)
		Weapon->Destroy();
}

/*
* GetHit 
*/

void AEnemy::GetHit_Implementation(const FVector& HitPoint)
{
	if (IsAlive())
	{
		EnemyState = EEnemyState::EES_HitReacting;

		GetWorldTimerManager().ClearAllTimersForObject(this);
		ShowHealthBar();
	}

	Super::GetHit_Implementation(HitPoint);
}

/*
* Die
*/

void AEnemy::Die_Implementation()
{
	if (IsTerminal())		// Terminal state is needed to allow multiple hits after death
	{
		HideHealthBar();
		GetWorldTimerManager().ClearAllTimersForObject(this);
		StopAIController();
		DisableCollisionsToDie();
		FadeOut();
		SetSpawnSoulTimer();

		EnemyState = EEnemyState::EES_Dead;
	}
	else if (IsDead())
		DeathIndex = DeathAnimationAfterDead_index;

	PlayDeathMontage();
}

void AEnemy::StopAIController()
{
	if (AIController)
	{
		AIController->StopMovement();
		AIController->ClearFocus(EAIFocusPriority::Gameplay);
	}
}

int16 AEnemy::PlayDeathMontage()
{
	int16 PlayedDeathIndex = Super::PlayDeathMontage();
	TEnumAsByte<EDeathPose> PlayedDeathPose(PlayedDeathIndex);
	if (PlayedDeathPose < EDeathPose::EDP_MAX)
		DeathPose = PlayedDeathPose;

	return PlayedDeathIndex;
}

void AEnemy::FadeOut()
{
	if (GetMesh()) GetMesh()->SetMaterial(0, DitheringMaterial);
	if (DitheringMaterial) DitheringMaterial->SetScalarParameterValue("totalDitheringTime", Dithering_totalTime);
	
	Dithering_startTime = GetWorld()->GetTimeSeconds();

	GetWorldTimerManager().SetTimer<AEnemy>(TimerHandler_dithering, this, &AEnemy::DecreaseDitheringOnMaterial, Dithering_execRate, true, Dithering_initialDelay);
	GetWorldTimerManager().SetTimer<AEnemy>(TimerHandler_deathPetals, this, &AEnemy::ActivateDeathPetalsAnim, DeathPetals_initialDelay);

	SetLifeSpan(Dithering_initialDelay + Dithering_totalTime + Dithering_extraTime);
}

void AEnemy::DecreaseDitheringOnMaterial()
{
	if (DitheringMaterial)
	{
		float DitheringCurrentTime = Dithering_startTime + Dithering_initialDelay;
		DitheringMaterial->SetScalarParameterValue("elapsedTime", GetWorld()->GetTimeSeconds() - DitheringCurrentTime);
	}
}

void AEnemy::ActivateDeathPetalsAnim()
{
	if (DeathPetals)
		DeathPetals->Activate(true);
}

void AEnemy::SetSpawnSoulTimer()
{
	float TimeToSpawn = Dithering_initialDelay + Dithering_totalTime - 2.f;
	GetWorldTimerManager().SetTimer(SpawnSoulTimer, this, &AEnemy::SpawnSoul, TimeToSpawn);
}

void AEnemy::SpawnSoul()
{
	if (GetWorld() && SoulClassToSpawn)
	{
		FVector CenterLocation = GetMesh()->GetBoneLocation(BoneToSpawnSoul);
		FVector LocationToSpawn = FVector(CenterLocation.X, CenterLocation.Y, CenterLocation.Z + 300.f);

		ASoul* SpawnedSoul = SpawnSoulAtLocation(LocationToSpawn);
		SetSoulsAmount(SpawnedSoul);
	}
}

ASoul* AEnemy::SpawnSoulAtLocation(FVector Location)
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	ASoul* SpawnedSoul = GetWorld()->SpawnActor<ASoul>(
		SoulClassToSpawn,
		Location,
		FRotator::ZeroRotator,
		SpawnParams);

	return SpawnedSoul;
}

void AEnemy::SetSoulsAmount(ASoul* Soul)
{
	int32 SoulsAmount = FMath::RandRange(MinSouls, MaxSouls);
	Soul->SetSoulsAmount(SoulsAmount);
}

/*
* Take damage
*/

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{	
	PawnSeen(EventInstigator->GetPawn());

	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

bool AEnemy::CanTakeDamage()
{
	return Super::CanTakeDamage() && 
		   HealthBar && !IsTerminal();
}

void AEnemy::ActuallyReceiveDamage(float DamageAmount)
{
	Super::ActuallyReceiveDamage(DamageAmount);

	if (!HasSomeHealthRemaining() && !IsDead() && !IsTerminal())
		EnemyState = EEnemyState::EES_Terminal;
}

void AEnemy::UpdateHealthBar()
{
	if (HealthBar && Attributes)
		HealthBar->SetPercentage(Attributes->GetHealthPercent());
}

//bool AEnemy::IsAwareOfCharacter()
//{
//	return EnemyState > EEnemyState::EES_Patrolling;
//}

/* 
* Tick
*/

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsTerminal() || IsDead()) return;

	UpdateLockOnWidget();

	if (IsPatrolling())
		CheckPatrol();
	else
		CheckCombat();	
}

/*
* LockOn
*/

void AEnemy::SetLockOnWidgetVisibility(bool bVisibility)
{
	UE_LOG(LogTemp, Warning, TEXT("show/hide lock on widget interface enemy! %d"), bVisibility)
	if (LockOnWidget)
		LockOnWidget->SetVisibility(bVisibility);
}

void AEnemy::UpdateLockOnWidget()
{
	if (LockOnWidget && LockOnWidget->IsVisible())
	{
		FVector StartPoint = LockOnWidget->GetComponentLocation();
		FVector TargetPoint = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->GetCameraLocation();

		FRotator RotationToFaceCamera = UKismetMathLibrary::FindLookAtRotation(StartPoint, TargetPoint);
		LockOnWidget->SetWorldRotation(RotationToFaceCamera);
	}
}

/*
* Combat: Pawn seen
*/

void AEnemy::PawnSeen(APawn* Pawn)
{
	if (!CanSee() || HasAlreadySeenTarget()) return;

	if (IsPawnAnAttackableCharacter(Pawn) && IsCharacterInsideCombatRange(Pawn))
	{
		ShowHealthBar();

		ClearPatrolTimer();

		SetCombatTarget(Pawn);
		SetFocalPointToActor(CombatTarget);
		ChaseCurrentTarget();
	}
}

bool AEnemy::CanSee()
{
	return !IsTerminal() && !IsDead();
}

bool AEnemy::HasAlreadySeenTarget()
{
	return EnemyState > EEnemyState::EES_Patrolling;
}

bool AEnemy::IsPawnAnAttackableCharacter(APawn* Pawn)
{
	return Pawn->ActorHasTag(FName("AttackableCharacter"));		// Casting is very expensive to be performed multiple times (ASlashCharacter* character = Cast<ASlashCharacter>(pawn)), so we use tags instead to check to pawn type!
}

bool AEnemy::IsCharacterInsideCombatRange(APawn* Pawn)
{
	return IsActorWithinRadius(Pawn, CombatRadius);
}

void AEnemy::ClearPatrolTimer()
{
	GetWorldTimerManager().ClearTimer(PatrolTimer);
}

void AEnemy::SetCombatTarget(AActor* Target)
{
	CombatTarget = Target;
}

/*
* Combat: main logic
*/

void AEnemy::CheckCombat()
{
	// This system of chasing/attacking/interest-loosing is designed as 2 concentric circles, defining attackRadius the smallast one, and combatRadius the biggest one.

	if (ShouldLoseInterest())
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
		SetFocalPointToActor(CombatTarget);

		SetAttackTimer();
	}
}

bool AEnemy::ShouldLoseInterest()
{
	return IsTargetOutOfRange() && !IsEngaged() ||
		   !IsTargetAttackable();
}

bool AEnemy::IsTargetOutOfRange()
{
	return !IsActorWithinRadius(CombatTarget, CombatRadius);
}

bool AEnemy::IsTargetAttackable()
{
	return CombatTarget->ActorHasTag(FName("AttackableCharacter"));
}

void AEnemy::ClearAttackTimer()
{
	GetWorldTimerManager().ClearTimer(AttackTimer);
}

void AEnemy::LoseInterest()
{
	CombatTarget = nullptr;

	ResetFocalPoint();

	EnemyState = EEnemyState::EES_Patrolling;
	GetCharacterMovement()->MaxWalkSpeed = GetPatrollingSpeed();
	MoveToTarget(CurrentPatrolTarget);
}

bool AEnemy::ShouldChaseCurrentTarget()
{
	return IsCharacterOutOfAttackRange() && !IsChasing() && !IsEngaged();
}

bool AEnemy::IsCharacterOutOfAttackRange()
{
	return !IsCharacterInsideAttackRange();
}

bool AEnemy::IsCharacterInsideAttackRange()
{
	return IsActorWithinRadius(CombatTarget, AttackRadius);
}

void AEnemy::ChaseCurrentTarget()
{
	EnemyState = EEnemyState::EES_Chasing;
	GetCharacterMovement()->MaxWalkSpeed = GetChasingSpeed();
	MoveToTarget(CombatTarget, AcceptanceRadius);
}

bool AEnemy::CanAttack()
{
	return IsCharacterInsideAttackRange() && 
		   !IsTerminal() && !IsAttacking() && !IsEngaged() && !IsHitReacting();
}

void AEnemy::SetAttackTimer()
{
	EnemyState = EEnemyState::EES_Attacking;

	const float AttackTime = FMath::RandRange(MinAttackWait, MaxAttackWait);
	GetWorldTimerManager().SetTimer(AttackTimer, this, &AEnemy::Attack, AttackTime);
}

void AEnemy::Attack()
{
	if (PlayAttackingMontage() != -1)
	{
		EnemyState = EEnemyState::EES_Engaged;
		ResetFocalPoint();
	}
}

void AEnemy::AttackEnd()
{
	EnemyState = EEnemyState::EES_NoState;
	CheckCombat();
}

void AEnemy::HitReactEnd()
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
		CurrentPatrolTarget = SelectNextPatrolTarget();
		MoveToNextPatrolTargetAfterSeconds(5.f);
	}
}

bool AEnemy::CanPatrol()
{
	return PatrolPoints.Num() > 0 && AIController != nullptr;
}

bool AEnemy::HasReachedCurrentTarget()
{
	return IsActorWithinRadius(CurrentPatrolTarget, PatrolRadius);
}

AActor* AEnemy::SelectNextPatrolTarget()
{
	TArray<AActor*> RemainingPatrolPoints;

	GetRemainingPatrolPoints(RemainingPatrolPoints);
	return GetRandomActorFromArray(RemainingPatrolPoints);
}

void AEnemy::GetRemainingPatrolPoints(TArray<AActor*>& RemainingPatrolPoints)
{
	for (AActor* actor : PatrolPoints)
	{
		if (actor != CurrentPatrolTarget)
			RemainingPatrolPoints.Add(actor);
	}
}

AActor* AEnemy::GetRandomActorFromArray(const TArray<AActor*>& RemainingPatrolPoints)
{
	if (RemainingPatrolPoints.Num() > 0)
	{
		const int16 RandomTargetIndex = FMath::RandRange(0, RemainingPatrolPoints.Num() - 1);
		return RemainingPatrolPoints[RandomTargetIndex];
	}

	return nullptr;
}

void AEnemy::MoveToNextPatrolTargetAfterSeconds(float Seconds)
{
	GetWorldTimerManager().SetTimer<AEnemy>(PatrolTimer, this, &AEnemy::MoveToPatrolTarget, Seconds);
}

void AEnemy::MoveToPatrolTarget()
{
	MoveToTarget(CurrentPatrolTarget);
}

/*
* General common methods
*/

bool AEnemy::IsAlive()
{
	return EnemyState > EEnemyState::EES_Terminal;
}

bool AEnemy::IsDead()
{
	return EnemyState == EEnemyState::EES_Dead;
}

bool AEnemy::IsTerminal()
{
	return EnemyState == EEnemyState::EES_Terminal;
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

bool AEnemy::IsHitReacting()
{
	return EnemyState == EEnemyState::EES_HitReacting;
}

void AEnemy::ShowHealthBar()
{
	if (HealthBar) HealthBar->SetVisibility(true);
}

void AEnemy::HideHealthBar()
{
	if (HealthBar) HealthBar->SetVisibility(false);
}

void AEnemy::SetFocalPointToActor(AActor* Actor)
{
	if (!AIController) return;

	if (Actor)
	{
		GetCharacterMovement()->bOrientRotationToMovement = false;
		GetCharacterMovement()->bUseControllerDesiredRotation = true;
		AIController->SetFocus(Actor);
	}
}

void AEnemy::ResetFocalPoint()
{
	if (!AIController) return;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	AIController->ClearFocus(EAIFocusPriority::Gameplay);
}

void AEnemy::MoveToTarget(const AActor* Target, float InAcceptanceRadius)
{
	if (!CanMoveToTarget(Target)) return;

	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalActor(Target);
	MoveRequest.SetAcceptanceRadius(InAcceptanceRadius);
	AIController->MoveTo(MoveRequest);
}

bool AEnemy::CanMoveToTarget(const AActor* Target)
{
	return Target != nullptr && AIController != nullptr;
}

float AEnemy::GetPatrollingSpeed()
{
	return Attributes? Attributes->GetWalkingSpeedEquipped() : -1.f;
}

float AEnemy::GetChasingSpeed()
{
	return Attributes? Attributes->GetRunningSpeedEquipped() : -1.f;
}

bool AEnemy::IsActorWithinRadius(AActor* Actor, float Radius)
{
	if (Actor == nullptr) return false;

	FVector ActorLocation = Actor->GetActorLocation();
	FVector SelfLocation = GetActorLocation();

	if (FMath::IsNearlyEqual(ActorLocation.X, SelfLocation.X) &&
		FMath::IsNearlyEqual(ActorLocation.Y, SelfLocation.Y) &&
		FMath::IsNearlyEqual(ActorLocation.Z, SelfLocation.Z))
		return true;

	return (Actor->GetActorLocation() - GetActorLocation()).Size() <= Radius;
}