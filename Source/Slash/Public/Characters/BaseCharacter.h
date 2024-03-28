// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"
#include "CharacterTypes.h"

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

protected:

	virtual void BeginPlay() override;

	/*
	* State
	*/

	EActionState actionState = EActionState::EAS_Unoccupied;

	/*
	* Attack
	*/

	virtual void Attack();
	
	UFUNCTION(BlueprintCallable)
	virtual void attackEnd();

	virtual bool CanAttack() { return false; }

	virtual void playAttackingMontage();

	UFUNCTION(BlueprintCallable)
	virtual void setWeaponCollision(ECollisionEnabled::Type collisionEnabled);

	virtual FName getHitDirection(const FVector& hitPoint);

	UPROPERTY(EditAnywhere, Category = "Indices|Attack")
	int8 attackIndex = -1;								// Use to test specific attack animations (if -1, it is not used and animations will be randomly selected, as designed)

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

	/*
	* Death
	*/

	virtual int8 playDeathMontage();
	virtual EDeathPose GetDeathPose(int8 inDeathIndex) { return EDeathPose::EDP_Dead1; }

	virtual void Die() {}

	UPROPERTY(EditAnywhere, Category = Montages)
	float frontBackAngle = 20.f;						// Base angle to calculate hit directions

	UPROPERTY(EditAnywhere, Category = "Indices|Montages")
	int8 deathIndex = -1;								// Use to test specific death animations (if -1, it is not used and animations will be randomly selected, as designed)

	/*
	* State
	*/

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<UAttributesComponent> attributes;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<AWeapon> weapon;

public:	

};
