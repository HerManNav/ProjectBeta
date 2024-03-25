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

	/*
	* Basic components
	*/

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<UHealthBarComponent> healthBar;

	UPROPERTY(VisibleDefaultsOnly)
	TObjectPtr<UNiagaraComponent> deathPetals;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UPawnSensingComponent> sensingComponent;

	/*
	* States
	*/

	UPROPERTY(BlueprintReadWrite)
	ELivingState livingState = ELivingState::ELS_Alive;

	UPROPERTY(BlueprintReadOnly)
	EEnemyState state = EEnemyState::EES_Patrolling;

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

	void combat();

	UFUNCTION()
	void seenPawn(APawn* pawn);

	void setFocalPointToActor(AActor* actor = nullptr);
	void resetFocalPoint();

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
	void showOrHideHealthBar();

	UFUNCTION(BlueprintCallable)
	bool isActorWithinRadius(AActor* target, float radius);

	void patrol();
	AActor* selectNextPatrolTarget();
	void moveToTarget(const AActor* target, float acceptanceRadius = 15.f);

	FTimerHandle patrolTimer;
	void finishedTimer();

	/*
	* Death
	*/

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UMaterialInterface> materialInstanceDynamic;

	UPROPERTY()								// Without this, this object would be considered for the GC, and then deleted in an uncontrolled way. So this prevents it and Unreal won't crash trying to access it after deleted (as it'd happen in fadeOut::decreaseDitheringOnMaterial method)
	TObjectPtr<UMaterialInstanceDynamic> ditheringMaterial;

	float dithering_startTime;				// Wall-clock time at which the timers for the fade out and deathPetals particles are set (e.g. second 10.f of the simulation)
	float dithering_initialDelay = 5.f;		// Time before start to fade out
	float dithering_totalTime = 3.f;		// Total time that takes to completa the fade out
	float dithering_execRate = 3.f / 100.f; // fadeOut() method is executed every dithering_execRate seconds
	float dithering_extraTime = 3.f;		// Time before destroying the enemy instance (extra time to let the Niagara effects finish)

	float deathPetals_initialDelay = 3.f;	// Time before start to play the deathPetals particles

	virtual ELivingState getDeathType(int8 deathIndex) override;

	virtual void die() override;

	UPROPERTY(EditAnywhere)
	int8 deathAnimationAfterDead_index = 1;	// This will trigger an animation for the enemy after die (e.g. 1 = "Death flying back" (see deathMontage) which is the most impressive one when enemy is on the floor)

	/**
	* Death fadeout
	*/

	void fadeOut();
	void decreaseDitheringOnMaterial();
	void activateDeathPetalsAnim();

	FTimerHandle timerHandler_dithering;
	FTimerHandle timerHandler_deathPetals;

public:

	ELivingState getLivingState() { return livingState; }

};
