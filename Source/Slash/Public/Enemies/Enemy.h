// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Characters/BaseCharacter.h"
#include "EnemyTypes.h"

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
	ECombatState combatState = ECombatState::ECS_Patrolling;

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

	UPROPERTY(VisibleDefaultsOnly)
	TObjectPtr<UMaterialInterface> materialInstanceDynamic;

	UPROPERTY()								// Without this, this object would be considered for the GC, and then deleted in an uncontrolled way. So this prevents it and Unreal won't crash trying to access it after deleted (as it'd happen in fadeOut::decreaseDitheringOnMaterial method)
	TObjectPtr<UMaterialInstanceDynamic> dynamicMaterial;

	float dithering_delay = 5.f;			// Time before start to fade out
	float dithering = 1.f;					// Current dithering percentage, which is updated every dithering_execRate till 0.f
	float dithering_fadeOutRate = 0.01f;	// Value considered in every update
	float dithering_execRate = 0.03;		// fadeOut() method is executed every dithering_execRate seconds
	float dithering_extraTime = 3.f;		// Time before destroying the enemy instance (extra time to let the Niagara effects finish)

	float deathPetals_delay = 3.f;			// Time before start to play the deathPetals particles

	virtual void playDeathMontage() override;
	virtual void die() override;
	void fadeOut();

public:

	ELivingState getLivingState() { return livingState; }

};
