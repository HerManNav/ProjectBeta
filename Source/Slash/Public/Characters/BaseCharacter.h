// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"
#include "Components/CapsuleComponent.h"
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

	/**
	* Methods
	*/

	/** Attack */
	virtual void Attack() {};
	virtual bool CanAttack() { return false; };
	virtual FName getHitDirection(const FVector& hitPoint);
	void PlayHitSoundAtLocation(const FVector& location);
	void PlayHitParticlesAtLocation(const FVector& location);

	/** Montage */
	virtual int16 PlayAttackingMontage();
	virtual int16 PlayDeathMontage();
	virtual void PlayMontage(UAnimMontage* Montage, FName MontageName);

	/** Death */
	virtual void Die() {}

	/** Exposed */

	UFUNCTION(BlueprintCallable)
	virtual void AttackEnd() {}

	UFUNCTION(BlueprintCallable)
	virtual void setWeaponCollision(ECollisionEnabled::Type collisionEnabled);

	/**
	* Variables
	*/

	/** State */

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<UAttributesComponent> attributes;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<AWeapon> weapon;

	/** Attack */

	UPROPERTY(EditAnywhere, Category = "Attack")
	float frontBackAngle = 20.f;						// Base angle to calculate hit directions

	UPROPERTY(EditAnywhere, Category = "Attack|Indices")
	int8 attackIndex = -1;								// Use to test specific attack animations (if -1, it is not used and animations will be randomly selected, as designed)

	UPROPERTY(EditAnywhere, Category = "Death|Indices")
	int8 deathIndex = -1;								// Use to test specific death animations (if -1, it is not used and animations will be randomly selected, as designed)

	UPROPERTY(EditAnywhere, Category = "Attack|Effects")
	TObjectPtr<USoundBase> hitSound;

	UPROPERTY(EditAnywhere, Category = "Attack|Effects")
	TObjectPtr<UParticleSystem> hitVFX;

	/** Montages */

	UPROPERTY(EditDefaultsOnly, Category = "Montages|Attack")
	TObjectPtr<UAnimMontage> AttackMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Montages|Attack")
	TObjectPtr<UAnimMontage> hitMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Montages|Death")
	TObjectPtr<UAnimMontage> deathMontage;

	UPROPERTY(EditAnywhere, Category = "Montages|Attack")
	TArray<FName> AttackMontageSectionNames;

	UPROPERTY(EditAnywhere, Category = "Montages|Death")
	TArray<FName> DeathMontageSectionNames;

private:

	virtual int16 PlayRandomMontageSection(UAnimMontage* Montage, TArray<FName> MontageSections);
	
};
