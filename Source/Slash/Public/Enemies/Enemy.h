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
	virtual void getHit_Implementation(const FVector& hitPoint) override;
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

	void SetFocalPointToActor(AActor* actor = nullptr);
	void ResetFocalPoint();

	void moveToTarget(const AActor* target, float acceptanceRadius = 15.f);

	float GetPatrollingSpeed();
	float GetChasingSpeed();

	/** Combat */
	void CheckCombat();
	bool ShouldLoseInterest();
	bool IsCharacterOutOfRange();
	bool IsCharacterOutOfAttackRange();
	bool IsCharacterInsideAttackRange();

	bool IsPawnMainCharacter(APawn* pawn);
	bool IsCharacterInsideCombatRange(APawn* pawn);

	void LoseInterest();
	void KeepPatrolling();
	bool ShouldChaseCurrentTarget();
	void ChaseCurrentTarget();
	void SetAttackTimer();
	void ClearAttackTimer();
	bool CanMoveToTarget(const AActor* target);

	/** Patrol */
	void CheckPatrol();
	bool CanPatrol();
	bool HasReachedCurrentTarget();
	AActor* selectNextPatrolTarget();
	void MoveToNextPatrolTargetAfterSeconds(float seconds);
	void MoveToPatrolTarget();
	void GetRemainingPatrolPoints(TArray<AActor*>& remainingPatrolPoints);
	AActor* GetRandomActorFromArray(const TArray<AActor*>& remainingPatrolPoints);
	void ClearPatrolTimer();	

	/** Take Damage */
	void PlayHitAnimationBasedOnHitPoint(const FVector& hitPoint);
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
	void PawnSeen(APawn* pawn);

	UFUNCTION(BlueprintCallable)
	bool isActorWithinRadius(AActor* target, float radius);

	/**
	* Variables
	*/

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UHealthBarComponent> healthBar;

	UPROPERTY(VisibleDefaultsOnly)
	TObjectPtr<UNiagaraComponent> deathPetals;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPawnSensingComponent> sensingComponent;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AWeapon> weaponClass;

	/** Combat (AI) */

	UPROPERTY()
	TObjectPtr<AActor> combatTarget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	float combatRadius = 1000.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	float attackRadius = 200.f;

	/** Patrol (AI) */

	UPROPERTY()
	TObjectPtr<AAIController> aiController;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = Patrol, meta = (AllowPrivateAccess = true))
	TObjectPtr<AActor> currentPatrolTarget;

	UPROPERTY(EditInstanceOnly, Category = Patrol)
	TArray<TObjectPtr<AActor>> patrolPoints;

	UPROPERTY(EditAnywhere)
	float patrolRadius = 200.f;		// Don't set this below 150! Just to be sure (should be always > (105 + 15 + 1) ; 105 is the variation in AAIController::MoveTo method, +15 i the acceptanceRadius and +1 is just in case for precision errors)

	FTimerHandle patrolTimer;

	/** Attack */

	UPROPERTY(EditAnywhere, Category = "Attack|Timers")
	float MinAttackWait = 1.f;

	UPROPERTY(EditAnywhere, Category = "Attack|Timers")
	float MaxAttackWait = 1.4f;

	FTimerHandle AttackTimer;

	/** Death */

	UPROPERTY(EditAnywhere)
	int8 deathAnimationAfterDead_index = 1;	// This will trigger an animation for the enemy after die if it gets hit again (e.g. 1 = "Death flying back" (see deathMontage) which is the most impressive one when enemy is on the floor)

	/** Death: Dithering */

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UMaterialInterface> materialInstanceDynamic;

	UPROPERTY()								// Without this, this object would be considered for the GC, and then deleted in an uncontrolled way. So this prevents it and Unreal won't crash trying to access it after deleted (as it'd happen in fadeOut::decreaseDitheringOnMaterial method)
	TObjectPtr<UMaterialInstanceDynamic> ditheringMaterial;

	FTimerHandle timerHandler_dithering;
	FTimerHandle timerHandler_deathPetals;

	float dithering_startTime;				// Wall-clock time at which the timers for the fade out and deathPetals particles are set (e.g. second 10.f of the simulation)
	float dithering_initialDelay = 5.f;		// Time before start to fade out
	float dithering_totalTime = 3.f;		// Total time that takes to complete the fade out
	float dithering_execRate = 3.f / 100.f; // fadeOut() method is executed every dithering_execRate seconds
	float dithering_extraTime = 3.f;		// Time before destroying the enemy instance (extra time to let the Niagara effects finish)

	float deathPetals_initialDelay = 3.f;	// Time before start to play the deathPetals particles

};
