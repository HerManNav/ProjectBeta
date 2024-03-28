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

	/*
	* Combat
	*/

	virtual void getHit_Implementation(const FVector& hitPoint) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

protected:

	virtual void BeginPlay() override;

	virtual void Destroyed() override;

	/*
	* Basic components
	*/

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<UHealthBarComponent> healthBar;

	UPROPERTY(VisibleDefaultsOnly)
	TObjectPtr<UNiagaraComponent> deathPetals;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UPawnSensingComponent> sensingComponent;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AWeapon> weaponClass;

	/*
	* State
	*/

	UPROPERTY(BlueprintReadOnly)
	EDeathPose DeathPose;

	UPROPERTY(BlueprintReadOnly)
	EEnemyState EnemyState = EEnemyState::EES_Patrolling;

private:

	/*
	* Combat (AI)
	*/

	UPROPERTY()
	TObjectPtr<AActor> combatTarget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	float combatRadius = 1000.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	float attackRadius = 200.f;

	void CheckCombat();

	UFUNCTION()
	void PawnSeen(APawn* pawn);

	void SetFocalPointToActor(AActor* actor = nullptr);
	void ResetFocalPoint();

	/*
	* Patrol (AI)
	*/

	UPROPERTY()
	TObjectPtr<AAIController> aiController;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = Patrol, meta = (AllowPrivateAccess = true))
	TObjectPtr<AActor> currentPatrolTarget;

	UPROPERTY(EditInstanceOnly, Category = Patrol)
	TArray<TObjectPtr<AActor>> patrolPoints;

	UPROPERTY(EditAnywhere)
	float patrolRadius = 200.f;		// Don't set this below 150! Just to be sure (should be always > (105 + 15 + 1) ; 105 is the variation in AAIController::MoveTo method, +15 i the acceptanceRadius and +1 is just in case for precision errors)

	UFUNCTION(BlueprintCallable)
	bool isActorWithinRadius(AActor* target, float radius);

	void CheckPatrol();
	AActor* selectNextPatrolTarget();
	void moveToTarget(const AActor* target, float acceptanceRadius = 15.f);

	FTimerHandle patrolTimer;
	void MoveToPatrolTarget();

	/*
	* Attack
	*/

	virtual bool CanAttack() override;

	UPROPERTY(EditAnywhere, Category = "Attack|Timers")
	float MinAttackWait = 0.5f;

	UPROPERTY(EditAnywhere, Category = "Attack|Timers")
	float MaxAttackWait = 1.f;

	FTimerHandle AttackTimer;

	/*
	* Death
	*/

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UMaterialInterface> materialInstanceDynamic;

	UPROPERTY()								// Without this, this object would be considered for the GC, and then deleted in an uncontrolled way. So this prevents it and Unreal won't crash trying to access it after deleted (as it'd happen in fadeOut::decreaseDitheringOnMaterial method)
	TObjectPtr<UMaterialInstanceDynamic> ditheringMaterial;

	float dithering_startTime;				// Wall-clock time at which the timers for the fade out and deathPetals particles are set (e.g. second 10.f of the simulation)
	float dithering_initialDelay = 5.f;		// Time before start to fade out
	float dithering_totalTime = 3.f;		// Total time that takes to complete the fade out
	float dithering_execRate = 3.f / 100.f; // fadeOut() method is executed every dithering_execRate seconds
	float dithering_extraTime = 3.f;		// Time before destroying the enemy instance (extra time to let the Niagara effects finish)

	float deathPetals_initialDelay = 3.f;	// Time before start to play the deathPetals particles

	virtual void Die() override;

	/**
	* Death fadeout
	*/

	void FadeOut();
	void DecreaseDitheringOnMaterial();
	void ActivateDeathPetalsAnim();

	FTimerHandle timerHandler_dithering;
	FTimerHandle timerHandler_deathPetals;

	/* Initialization */
	void InitHealthBar();
	void InitAI();
	void InitDeathMembers();
	void EquipWeapon();

	/* States */

	bool IsDead();
	bool IsAlive();
	bool IsPatrolling();
	bool IsChasing();
	bool IsAttacking();
	bool IsCharacterOutOfRange();
	bool IsCharacterOutOfAttackRange();
	bool IsCharacterInsideAttackRange();
	void ShowHealthBar();
	void HideHealthBar();

	/* Combat */
	bool IsPawnMainCharacter(APawn* pawn);
	bool IsCharacterInsideCombatRange(APawn* pawn);

	void LoseInterest();
	void Chase();
	virtual void Attack() override;
	void SetAttackTimer();
	void ClearAttackTimer();
	bool CanMoveToTarget(const AActor* target);

	/* Patrol */
	bool CanPatrol();
	bool HasReachedCurrentTarget();
	void MoveToNextPatrolTargetAfterSeconds(float seconds);
	void GetRemainingPatrolPoints(TArray<AActor*>& remainingPatrolPoints);
	AActor* GetRandomActorFromArray(const TArray<AActor*>& remainingPatrolPoints);
	void ClearPatrolTimer();

	/* Hit */
	void PlayHitAnimationBasedOnHitPoint(const FVector& hitPoint);
	void PlayHitSoundAtLocation(const FVector& location);
	void PlayHitParticlesAtLocation(const FVector& location);

	/* Take Damage */
	bool CanTakeDamage();
	void ActuallyReceiveDamage(float DamageAmount);
	void UpdateHealthBar();
	bool IsAwareOfCharacter();
	void SetCombatTarget(AActor* target);

	/* Death */
	void StopAIController();
	void DisableCollisionsForPawn();
	void PlayDeathAnimation();
	virtual EDeathPose GetDeathPose(int8 inDeathIndex) override;
};
