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
	void ReactToHitBasedOnHitDirection(const FVector& HitPoint);
	void PlayHitSoundAtLocation(const FVector& Location);
	void PlayHitParticlesAtLocation(const FVector& Location);

	/** Montage */
	virtual int16 PlayAttackingMontage();
	virtual int16 PlayDeathMontage();
	virtual void PlayMontage(UAnimMontage* Montage, FName SectionName);

	/** Death */
	virtual void Die() {}

	/** Exposed */

	UFUNCTION(BlueprintCallable)
	virtual void AttackEnd() {}

	UFUNCTION(BlueprintCallable)
	virtual void SetWeaponCollision(ECollisionEnabled::Type CollisionEnabled);

	/**
	* Variables
	*/

	/** State */

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<UAttributesComponent> Attributes;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<AWeapon> Weapon;

	/** Attack */

	UPROPERTY(EditAnywhere, Category = "Attack|Indices")
	int8 AttackIndex = -1;								// Use to test specific attack animations (if -1, it is not used and animations will be randomly selected, as designed)

	UPROPERTY(EditAnywhere, Category = "Death|Indices")
	int8 DeathIndex = -1;								// Use to test specific death animations (if -1, it is not used and animations will be randomly selected, as designed)

private:

	int16 PlayRandomMontageSection(UAnimMontage* Montage, TArray<FName> MontageSections, int8 SectionIndexDefault = -1);	// If SectionIndexDefault != -1, it won't play a random animation but the indicated one
	FName GetHitDirection(const FVector& HitPoint);

	/*
	* Variables
	*/

	/** Attack */

	UPROPERTY(EditAnywhere, Category = "Attack")
	float FrontBackAngle = 20.f;						// Base angle to calculate hit directions

	UPROPERTY(EditAnywhere, Category = "Attack|Effects")
	TObjectPtr<USoundBase> HitSound;

	UPROPERTY(EditAnywhere, Category = "Attack|Effects")
	TObjectPtr<UParticleSystem> HitVFX;

	/** Montages */

	UPROPERTY(EditDefaultsOnly, Category = "Montages|Attack")
	TObjectPtr<UAnimMontage> AttackMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Montages|Attack")
	TObjectPtr<UAnimMontage> HitMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Montages|Death")
	TObjectPtr<UAnimMontage> deathMontage;

	UPROPERTY(EditAnywhere, Category = "Montages|Attack")
	TArray<FName> AttackMontageSectionNames;

	UPROPERTY(EditAnywhere, Category = "Montages|Death")
	TArray<FName> DeathMontageSectionNames;
	
};
