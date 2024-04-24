// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/TimelineComponent.h"
//#include "Curves/CurveFloat.h"

#include "LockOnComponent.generated.h"

class UCameraComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SLASH_API ULockOnComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	ULockOnComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void Init(ACharacter* InOwner, UCameraComponent* Camera);

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

	FRotator GetFocusToTargetRotation();

	UFUNCTION()
	void LerpControllerRotation(float Alpha);

	/*
	* Variables
	*/

	/** Inherit from owner */

	UPROPERTY()
	TObjectPtr<UCameraComponent> ViewCamera;

	UPROPERTY()
	TObjectPtr<ACharacter> Owner;

	/** LockOn variables */

	/** If bActivated = false, the whole LockOn system will be disabled */
	UPROPERTY(EditDefaultsOnly, Category = "LockOn Properties")
	bool bActivated = true;

	UPROPERTY(EditDefaultsOnly, Category = "LockOn Properties")
	bool bDrawDebug = true;

	UPROPERTY(EditDefaultsOnly, Category = "LockOn Properties")
	float TraceLength = 1000.f;

	UPROPERTY(EditDefaultsOnly, Category = "LockOn Properties")
	float DistanceToDeactivateLockOn = 200.f;

	UPROPERTY(EditDefaultsOnly, Category = "LockOn Properties|Timeline")
	class UCurveFloat* TimelineCurve;

	UPROPERTY(EditDefaultsOnly, Category = "LockOn Properties")
	class UCurveFloat* CameraTiltCurve;

	UPROPERTY(EditDefaultsOnly, Category = "LockOn Properties")
	class UCurveFloat* CameraRightDisplacementCurve;

private:

	UPROPERTY(VisibleAnywhere, Category = "LockOn Properties")
	int16 CurrentTargetIndex = -1;

	UPROPERTY(VisibleAnywhere, Category = "LockOn Properties")
	TArray<TObjectPtr<AActor>> Targets;

	UPROPERTY(VisibleAnywhere, Category = "LockOn Properties")
	TObjectPtr<AActor> CurrentTarget;

	UPROPERTY(VisibleAnywhere, Category = "LockOn Properties")
	float DistanceToCurrentEnemy;

	UPROPERTY(VisibleAnywhere, Category = "LockOn Properties|Timeline")
	FTimeline RotationTimeline;

};