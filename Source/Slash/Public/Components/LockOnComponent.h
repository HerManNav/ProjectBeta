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

	int16 Enable();
	void Disable();
	void SwapTarget();

protected:

	virtual void BeginPlay() override;

	/*
	* Methods
	*/

	void GetLockOnTargets(TArray<AActor*>& InTargets);
	float GetDistanceToEnemyAtIndex(const TArray<AActor*>& Enemies, int16 Index);

	void GetHitsToPawnsInFront(TArray<FHitResult> PawnHits);
	TArray<AActor*> GetVisibleImpactedActors(TArray<FHitResult> PawnHits);
	void DoSphereTrace(const FVector& PawnHitLocation, FHitResult& HitResult);

	/*
	* Variables
	*/

	/** Inherit from owner */

	UPROPERTY()
	TObjectPtr<UCameraComponent> ViewCamera;

	FVector OwnerLocation;

	/** LockOn variables */

	UPROPERTY(EditDefaultsOnly, Category = "LockOn")
	float TraceLength = 1000.f;

	UPROPERTY(EditDefaultsOnly, Category = "LockOn")
	bool bDrawDebug = true;

	UPROPERTY(VisibleAnywhere, Category = "LockOn")
	int16 CurrentTargetIndex = -1;

	UPROPERTY(VisibleAnywhere, Category = "LockOn")
	TArray<TObjectPtr<AActor>> Targets;

	UPROPERTY(VisibleAnywhere, Category = "LockOn")
	float DistanceToCurrentEnemy;

};
