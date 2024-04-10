// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Characters/BaseCharacter.h"

#include "Enemy.generated.h"

class ASoul;

class UHealthBarComponent;
class UNiagaraComponent;

class AAIController;
class UPawnSensingComponent;

UCLASS()
class SLASH_API AEnemy : public ABaseCharacter
{
	GENERATED_BODY()

public:

	AEnemy();

	virtual void Tick(float DeltaTime) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	virtual void Destroyed() override;

	/** <IHitInterface> */
	virtual void GetHit_Implementation(const FVector& HitPoint) override;
	/** </IHitInterface> */

protected:

	virtual void BeginPlay() override;

	/** <ABaseCharacter> */
	virtual bool CanAttack() override;
	virtual void Attack() override;
	virtual void AttackEnd() override;
	virtual void HitReactEnd() override;
	virtual void Die() override;
	virtual int16 PlayDeathMontage() override;
	virtual bool CanTakeDamage() override;
	virtual void ActuallyReceiveDamage(float DamageAmount) override;
	virtual void UpdateHealthBar() override;
	/** </ABaseCharacter> */

	UPROPERTY(BlueprintReadOnly)
	EEnemyState EnemyState = EEnemyState::EES_Patrolling;

private:

	/**
	* Methods
	*/

	/** Initialization */
	void InitHealthBar();
	void InitAI();
	void InitDeathMembers();
	void SpawnAndEquipWeapon();

	/** States and general methods */

	bool IsAlive();
	bool IsDead();
	bool IsTerminal();
	bool IsPatrolling();
	bool IsChasing();
	bool IsAttacking();
	bool IsEngaged();
	bool IsHitReacting();

	void ShowHealthBar();
	void HideHealthBar();

	void SetFocalPointToActor(AActor* Actor = nullptr);
	void ResetFocalPoint();

	void MoveToTarget(const AActor* Target, float AcceptanceRadius = 15.f);

	float GetPatrollingSpeed();
	float GetChasingSpeed();

	/** Combat */
	void CheckCombat();
	bool ShouldLoseInterest();
	bool IsTargetOutOfRange();
	bool IsTargetAttackable();
	bool IsCharacterOutOfAttackRange();
	bool IsCharacterInsideAttackRange();

	bool CanSee();
	bool HasAlreadySeenTarget();
	bool IsPawnAnAttackableCharacter(APawn* Pawn);
	bool IsCharacterInsideCombatRange(APawn* Pawn);
	void SetCombatTarget(AActor* target);

	void LoseInterest();
	bool ShouldChaseCurrentTarget();
	void ChaseCurrentTarget();
	void SetAttackTimer();
	void ClearAttackTimer();
	bool CanMoveToTarget(const AActor* Target);

	/** Patrol */
	void CheckPatrol();
	bool CanPatrol();
	bool HasReachedCurrentTarget();
	AActor* SelectNextPatrolTarget();
	void MoveToNextPatrolTargetAfterSeconds(float Seconds);
	void MoveToPatrolTarget();
	void GetRemainingPatrolPoints(TArray<AActor*>& RemainingPatrolPoints);
	AActor* GetRandomActorFromArray(const TArray<AActor*>& RemainingPatrolPoints);
	void ClearPatrolTimer();

	/** Death */
	void StopAIController();
	void FadeOut();
	void DecreaseDitheringOnMaterial();
	void ActivateDeathPetalsAnim();
	void SetSpawnSoulTimer();
	void SpawnSoul();

	/** Exposed */

	UFUNCTION()
	void PawnSeen(APawn* Pawn);

	UFUNCTION(BlueprintCallable)
	bool IsActorWithinRadius(AActor* Target, float Radius);

	/**
	* Variables
	*/

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UHealthBarComponent> HealthBar;

	UPROPERTY(VisibleDefaultsOnly)
	TObjectPtr<UNiagaraComponent> DeathPetals;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UPawnSensingComponent> SensingComponent;

	UPROPERTY(EditDefaultsOnly, Category = Combat)
	TSubclassOf<AWeapon> WeaponClass;

	/** AI: Combat */

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat|Radius", meta = (AllowPrivateAccess = true))
	float CombatRadius = 1000.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat|Radius", meta = (AllowPrivateAccess = true))
	float AttackRadius = 200.f;

	/** AI: Patrol */

	UPROPERTY()
	TObjectPtr<AAIController> AIController;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Combat|Patrol", meta = (AllowPrivateAccess = true))
	TObjectPtr<AActor> CurrentPatrolTarget;

	UPROPERTY(EditInstanceOnly, Category = "Combat|Patrol")
	TArray<TObjectPtr<AActor>> PatrolPoints;

	UPROPERTY(EditAnywhere, Category = "Combat|Patrol")
	float PatrolRadius = 200.f;		// Don't set this below 150! Just to be sure (should be always > (105 + 15 + 1) ; 105 is the variation in AAIController::MoveTo method, +15 i the acceptanceRadius and +1 is just in case for precision errors)

	FTimerHandle PatrolTimer;

	/** AI: Attack */

	UPROPERTY(EditAnywhere, Category = "Combat|Attack|Timers")
	float MinAttackWait = 0.8f;

	UPROPERTY(EditAnywhere, Category = "Combat|Attack|Timers")
	float MaxAttackWait = 1.2f;

	FTimerHandle AttackTimer;

	/** Death */

	UPROPERTY(EditAnywhere, Category = "Combat|Death")
	int8 DeathAnimationAfterDead_index = 1;	// This will trigger an animation for the enemy after die if it gets hit again (e.g. 1 = "Death flying back" (see DeathMontage) which is the most impressive one when enemy is on the floor)

	/** Death: Dithering */

	UPROPERTY(EditDefaultsOnly, Category = "Combat|Death")
	TObjectPtr<UMaterialInterface> AfterDeathDitheringMI;

	UPROPERTY()								// Without this, this object would be considered for the GC, and then deleted in an uncontrolled way. So this prevents it and Unreal won't crash trying to access it after deleted (as it'd happen in fadeOut::decreaseDitheringOnMaterial method)
	TObjectPtr<UMaterialInstanceDynamic> DitheringMaterial;

	FTimerHandle TimerHandler_dithering;
	FTimerHandle TimerHandler_deathPetals;

	float Dithering_startTime;				// Wall-clock time at which the timers for the fade out and DeathPetals particles are set (e.g. second 10.f of the simulation)
	float Dithering_initialDelay = 5.f;		// Time before start to fade out
	float Dithering_totalTime = 3.f;		// Total time that takes to complete the fade out
	float Dithering_execRate = 3.f / 100.f; // fadeOut() method is executed every dithering_execRate seconds
	float Dithering_extraTime = 3.f;		// Time before destroying the enemy instance (extra time to let the Niagara effects finish)

	float DeathPetals_initialDelay = 3.f;	// Time before start to play the DeathPetals particles

	/** Death Souls */

	FTimerHandle SpawnSoulTimer;

	UPROPERTY(EditAnywhere, Category = "Souls")
	TSubclassOf<ASoul> SoulClassToSpawn;
};
