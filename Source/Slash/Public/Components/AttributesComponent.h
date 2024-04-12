// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttributesComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SLASH_API UAttributesComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	UAttributesComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:

	/** Health & Stamina */

	UPROPERTY(EditAnywhere, Category = Attributes, meta = (ClampMin = "0"))
	float Health = 100.f;

	UPROPERTY(EditAnywhere, Category = Attributes, meta = (ClampMin = "0"))
	float MaxHealth = 100.f;

	UPROPERTY(EditAnywhere, Category = Attributes, meta = (ClampMin = "0"))
	float Stamina = 100.f;

	UPROPERTY(EditAnywhere, Category = Attributes, meta = (ClampMin = "0"))
	float MaxStamina = 100.f;

	UPROPERTY(EditAnywhere, Category = Attributes)
	float HealthRecoveryRate_Secs = 1.f;

	UPROPERTY(EditAnywhere, Category = Attributes)
	float StaminaRecoveryRate_Secs = 3.f;

	/** Walk/Run speeds */

	UPROPERTY(EditAnywhere, Category = Movement)
	float WalkingSpeedUnequipped = 200.f;

	UPROPERTY(EditAnywhere, Category = Movement)
	float RunningSpeedUnequipped = 400.f;

	UPROPERTY(EditAnywhere, Category = Movement)
	float WalkingSpeedEquipped = 125.f;

	UPROPERTY(EditAnywhere, Category = Movement)
	float RunningSpeedEquipped = 300.f;

	UPROPERTY(EditAnywhere, Category = Inventory)
	int32 GoldAmount = 0;

	UPROPERTY(EditAnywhere, Category = Inventory)
	int32 SoulsAmount = 0;

public:

	bool IsAlive();

	void ReceiveDamage(float Damage);
	void RecoverHealth(float HealthAmount);
	void ConsumeStamina(float StaminaConsumption);
	void RecoverStamina(float StaminaAmount);
	
	bool IsHealthMaxedOut();
	bool IsStaminaMaxedOut();

	void AddGold(int32 InGoldAmount);
	void AddSouls(int32 InSoulsAmount);

	/*
	* Getters
	*/

	/** Health & Stamina */

	FORCEINLINE float GetHealth() { return Health; }

	FORCEINLINE float GetStamina() { return Stamina; }

	FORCEINLINE float GetHealthPercent() { return Health / MaxHealth; }

	FORCEINLINE float GetStaminaPercent() { return Stamina / MaxStamina; }

	FORCEINLINE float GetHealthRecoveryRate() { return HealthRecoveryRate_Secs; }

	FORCEINLINE float GetStaminaRecoveryRate() { return StaminaRecoveryRate_Secs; }

	/** Walk/Run speeds */

	FORCEINLINE float GetWalkingSpeedUnequipped() { return WalkingSpeedUnequipped; }

	FORCEINLINE float GetRunningSpeedUnequipped() { return RunningSpeedUnequipped; }

	FORCEINLINE float GetWalkingSpeedEquipped() { return WalkingSpeedEquipped; }

	FORCEINLINE float GetRunningSpeedEquipped() { return RunningSpeedEquipped; }

	/** Gold & Souls */

	FORCEINLINE int32 GetGoldAmount() { return GoldAmount; }

	FORCEINLINE int32 GetSoulsAmount() { return SoulsAmount; }

	/*
	* Setters
	*/

	void SetHealthRecoveryRate(float InHealthRecoveryRate) { HealthRecoveryRate_Secs = InHealthRecoveryRate; }

	void SetStaminaRecoveryRate(float InStaminaRecoveryRate) { StaminaRecoveryRate_Secs = InStaminaRecoveryRate; }
};
