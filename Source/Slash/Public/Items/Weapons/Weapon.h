// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Weapon.generated.h"

class USoundBase;
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
	void Equip(USceneComponent* InParent, FName InSocketName, AActor* newOwner, APawn* newInstigator);

	void AttachMeshToSocket(USceneComponent* InParent, const FName& InSocketName);
	
protected:
	virtual void BeginPlay() override;

	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	virtual void OnSphereOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

	UFUNCTION()
	void OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintImplementableEvent)
	void createHitFields(const FVector& location);

	/*
	* Damage related members
	*/

	float baseDamage = 20.f;

	float damageVariation = 10.f;

private:

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	USoundBase* EquipSound;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	UBoxComponent* WeaponBox;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* BoxTraceStart;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* BoxTraceEnd;

	TArray<AActor*> actorsToIgnore;

public:

	/*
	* Getters & setters
	*/

	void setBoxCollision(ECollisionEnabled::Type collisionEnabled);		// Need to define this in the .cpp because we do stuff with UBoxComponent, which is not included in this .h file!

	void clearActorsToIgnore() { actorsToIgnore.Empty(); };

};