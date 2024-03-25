// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"
#include "BaseCharacter.generated.h"

class UAnimMontage;
class AWeapon;

class UAttributesComponent;

UCLASS()
class SLASH_API ABaseCharacter : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:

	ABaseCharacter();

	virtual void Tick(float DeltaTime) override;

	/*
	* Combat
	*/

	virtual void getHit_Implementation(const FVector& hitPoint) override;

protected:

	virtual void BeginPlay() override;

	/*
	* Attack
	*/

	virtual void attack();

	virtual bool canAttack();

	virtual void playAttackingMontage();

	UFUNCTION(BlueprintCallable)
	virtual void endAttack();

	UFUNCTION(BlueprintCallable)
	virtual void setWeaponCollision(ECollisionEnabled::Type collisionEnabled);

	virtual FName getHitDirection(const FVector& hitPoint);

	/*
	* SFXs and VFXs
	*/

	UPROPERTY(EditAnywhere, Category = Sound)
	TObjectPtr<USoundBase> hitSound;

	UPROPERTY(EditAnywhere, Category = Particles)
	TObjectPtr<UParticleSystem> hitVFX;

	/*
	* Montages
	*/

	virtual void playMontage(UAnimMontage* montage, FName montageName);

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	TObjectPtr<UAnimMontage> attackMontage;

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	TObjectPtr<UAnimMontage> hitMontage;

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	TObjectPtr<UAnimMontage> deathMontage;

	UPROPERTY(EditAnywhere, Category = Montages)
	float frontBackAngle = 20.f;						// Base angle to calculate hit directions

	UPROPERTY(EditAnywhere, Category = Montages, meta = (ClampMin = "-1", ClampMax = "2"))
	int8 deathIndex = -1;

	/*
	* Death
	*/

	virtual void playDeathMontage();

	virtual void die();

	/*
	* State
	*/

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<UAttributesComponent> attributes;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<AWeapon> weapon;

public:	

};
