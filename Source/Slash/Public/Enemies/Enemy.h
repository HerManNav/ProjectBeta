// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Characters/BaseCharacter.h"

#include "Enemy.generated.h"

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
	virtual void Die() override;
	virtual int16 PlayDeathMontage() override;
	/** </ABaseCharacter> */

	UPROPERTY(BlueprintReadOnly)
	TEnumAsByte<EDeathPose> DeathPose;

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
	bool IsCharacterOutOfRange();
	bool IsCharacterOutOfAttackRange();
	bool IsCharacterInsideAttackRange();

	bool IsPawnMainCharacter(APawn* Pawn);
	bool IsCharacterInsideCombatRange(APawn* Pawn);

	void LoseInterest();
	void KeepPatrolling();
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

	/** Take Damage */
	bool CanTakeDamage();
	void ActuallyReceiveDamage(float DamageAmount);
	bool HasSomeHealthRemaining();
	void UpdateHealthBar();
	bool IsAwareOfCharacter();
	void SetCombatTarget(AActor* target);

	/** Death */
	void StopAIController();
	void DisableCollisionsForPawn();
	void FadeOut();
	void DecreaseDitheringOnMaterial();
	void ActivateDeathPetalsAnim();

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

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPawnSensingComponent> SensingComponent;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AWeapon> WeaponClass;

	/** Combat (AI) */

	UPROPERTY()
	TObjectPtr<AActor> CombatTarget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	float CombatRadius = 1000.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	float AttackRadius = 200.f;

	/** Patrol (AI) */

	UPROPERTY()
	TObjectPtr<AAIController> AIController;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = Patrol, meta = (AllowPrivateAccess = true))
	TObjectPtr<AActor> CurrentPatrolTarget;

	UPROPERTY(EditInstanceOnly, Category = Patrol)
	TArray<TObjectPtr<AActor>> PatrolPoints;

	UPROPERTY(EditAnywhere)
	float PatrolRadius = 200.f;		// Don't set this below 150! Just to be sure (should be always > (105 + 15 + 1) ; 105 is the variation in AAIController::MoveTo method, +15 i the acceptanceRadius and +1 is just in case for precision errors)

	FTimerHandle PatrolTimer;

	/** Attack */

	UPROPERTY(EditAnywhere, Category = "Attack|Timers")
	float MinAttackWait = 1.f;

	UPROPERTY(EditAnywhere, Category = "Attack|Timers")
	float MaxAttackWait = 1.4f;

	FTimerHandle AttackTimer;

	/** Death */

	UPROPERTY(EditAnywhere)
	int8 DeathAnimationAfterDead_index = 1;	// This will trigger an animation for the enemy after die if it gets hit again (e.g. 1 = "Death flying back" (see deathMontage) which is the most impressive one when enemy is on the floor)

	/** Death: Dithering */

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UMaterialInterface> MaterialInstanceDynamic;

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

};
