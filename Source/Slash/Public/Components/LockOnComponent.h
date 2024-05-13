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

	/** Returns the number of found targets */
	int16 Enable();
	void Disable();
	void SwapTarget();

	void UpdateMovementVector(float MovementDirection);

	void Pause();
	void Resume();

protected:

	virtual void BeginPlay() override;

	/*
	* Methods
	*/

	void GetLockOnTargets(TArray<AActor*>& InTargets);
	void GetHitsToPawnsInFront(TArray<FHitResult>& OutPawnHits);
	void GetVisibleImpactedActors(TArray<AActor*>& OutVisiblePawns, const TArray<FHitResult>& PawnHits);
	void DoLineTraceAgainstWorldStatic(FHitResult& OutHitResult, const FVector& TraceStart, const FVector& TraceEnd);

	void ShowLockOnWidgetOnActor(AActor* Actor);
	void HideLockOnWidgetOnActor(AActor* Actor);

	int16 SelectNextIndex();

	bool IsLockOnActive();

	float GetDistanceToTarget(const AActor* Target);
	FRotator GetFocusToTargetRotation();
	void LerpControllerRotation(float Alpha);
	void LerpCharacterYawRotation(float Alpha);

	UFUNCTION()
	void LerpRotationToFaceEnemy(float Alpha);

	/*
	* Variables
	*/

	/** Inherit from owner */

	UPROPERTY()
	TObjectPtr<UCameraComponent> ViewCamera;

	UPROPERTY()
	TObjectPtr<ACharacter> Owner;

	UPROPERTY()
	TObjectPtr<AActor> ControlledActor;

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

	UPROPERTY(VisibleAnywhere, Category = "LockOn Properties")
	FRotator FocusToTargetRotation;

	UPROPERTY(VisibleAnywhere, Category = "LockOn Properties|Timeline")
	FTimeline RotationTimeline;

	UPROPERTY(VisibleAnywhere, Category = "LockOn Properties")
	float MovementDirectionDeg;

public:

	/** Returns the movement direction in degrees */
	FORCEINLINE float GetMovementDirection() const { return MovementDirectionDeg; }

};
