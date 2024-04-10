// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Weapon.generated.h"

class UBoxComponent;

/**
 *
 */
UCLASS()
class SLASH_API AWeapon : public AItem
{
	GENERATED_BODY()

public:

	AWeapon();

	UFUNCTION(BlueprintCallable)
	void Equip(USceneComponent* Parent, FName SocketName, AActor* NewOwner, APawn* NewInstigator);
	
protected:

	virtual void BeginPlay() override;
	
	/**
	* Methods
	*/

	UFUNCTION()
	void OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintImplementableEvent)
	void CreateHitFields(const FVector& Location);

	/**
	* Variables
	*/

	/** Damage related members */

	UPROPERTY(EditAnywhere, Category = "Debug|Box trace")
	bool bShowDebugBoxTrace = false;

	UPROPERTY(EditAnywhere, Category = "Damage")
	float BaseDamage = 20.f;

	UPROPERTY(EditAnywhere, Category = "Damage")
	float DamageVariation = 10.f;

private:

	/**
	* Methods
	*/

	void DeactivateNiagara();

	/** Box trace */
	bool ShouldPerformBoxTrace(AActor* OverlappedActor);
	void PerformBoxTrace(FHitResult& BoxHit);
	void ApplyDamage(const FHitResult& BoxHit);
	void ApplyHit(const FHitResult& BoxHit);

	/**
	* Variables
	*/

	TArray<AActor*> ActorsToIgnore;

	/** Components */

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBoxComponent> WeaponBox;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> BoxTraceStart;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> BoxTraceEnd;

public:

	/*
	* Getters & setters
	*/

	void SetBoxCollision(ECollisionEnabled::Type CollisionEnabled);		// Need to define this in the .cpp because we do stuff with UBoxComponent, which is not included in this .h file!

	void ClearActorsToIgnore() { ActorsToIgnore.Empty(); };

};