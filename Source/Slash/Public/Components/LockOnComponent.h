// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LockOnComponent.generated.h"

class UCameraComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SLASH_API ULockOnComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	ULockOnComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void Init(UCameraComponent* Camera);

	int16 Enable();		// Returns the number of found targets
	void Disable();
	void SwapTarget();

protected:

	virtual void BeginPlay() override;

	/*
	* Methods
	*/

	void GetLockOnTargets(TArray<AActor*>& InTargets);
	void GetHitsToPawnsInFront(TArray<FHitResult>& OutPawnHits);
	void GetVisibleImpactedActors(TArray<AActor*>& OutVisiblePawns, const TArray<FHitResult>& PawnHits);
	void DoLineTraceAgainstWorldStatic(FHitResult& OutHitResult, const FVector& TraceStart, const FVector& TraceEnd);

	float GetDistanceToTarget(const AActor* Target);

	void ShowLockOnWidgetOnActor(AActor* Actor);
	void HideLockOnWidgetOnActor(AActor* Actor);
	void StopRotationTimeline();

	/*
	* Variables
	*/

	/** Inherit from owner */

	UPROPERTY()
	TObjectPtr<UCameraComponent> ViewCamera;

	FVector OwnerLocation;

	/** LockOn variables */

	/** If bActivate = false, the whole LockOn system will be disabled */
	UPROPERTY(EditDefaultsOnly, Category = "LockOn Properties")
	bool bActivate = true;

	UPROPERTY(EditDefaultsOnly, Category = "LockOn Properties")
	bool bDrawDebug = true;

	UPROPERTY(EditDefaultsOnly, Category = "LockOn Properties")
	float TraceLength = 1000.f;

	UPROPERTY(VisibleAnywhere, Category = "LockOn Properties")
	int16 CurrentTargetIndex = -1;

	UPROPERTY(VisibleAnywhere, Category = "LockOn Properties")
	TArray<TObjectPtr<AActor>> Targets;

	UPROPERTY(VisibleAnywhere, Category = "LockOn Properties")
	TObjectPtr<AActor> CurrentTarget;

	UPROPERTY(VisibleAnywhere, Category = "LockOn Properties")
	float DistanceToCurrentEnemy;

};
