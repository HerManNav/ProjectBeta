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

	int32 GoldAmount = 0;

	int32 SoulsAmount = 0;

	UPROPERTY(EditAnywhere, Category = Attributes, meta = (ClampMin = "0"))
	float Health = 100.f;

	UPROPERTY(EditAnywhere, Category = Attributes, meta = (ClampMin = "0"))
	float MaxHealth = 100.f;

	UPROPERTY(EditAnywhere, Category = Attributes, meta = (ClampMin = "0"))
	float Stamina = 100.f;

	UPROPERTY(EditAnywhere, Category = Attributes, meta = (ClampMin = "0"))
	float MaxStamina = 100.f;

	UPROPERTY(EditAnywhere, Category = Movement)
	float WalkingSpeedUnequipped = 200.f;

	UPROPERTY(EditAnywhere, Category = Movement)
	float RunningSpeedUnequipped = 400.f;

	UPROPERTY(EditAnywhere, Category = Movement)
	float WalkingSpeedEquipped = 125.f;

	UPROPERTY(EditAnywhere, Category = Movement)
	float RunningSpeedEquipped = 300.f;

public:

	bool IsAlive();

	void ReceiveDamage(float Damage);
	void ConsumeStamina(float StaminaConsumption);
	void RecoverStamina(float StaminaRecover);

	void AddGold(int32 InGoldAmount);
	void AddSouls(int32 InSoulsAmount);

	/*
	* Getters and setters
	*/

	FORCEINLINE float GetHealthPercent() { return Health / MaxHealth; }

	FORCEINLINE float GetStamina() { return Stamina; }

	FORCEINLINE float GetStaminaPercent() { return Stamina / MaxStamina; }

	FORCEINLINE float GetWalkingSpeedUnequipped() { return WalkingSpeedUnequipped; }

	FORCEINLINE float GetRunningSpeedUnequipped() { return RunningSpeedUnequipped; }

	FORCEINLINE float GetWalkingSpeedEquipped() { return WalkingSpeedEquipped; }

	FORCEINLINE float GetRunningSpeedEquipped() { return RunningSpeedEquipped; }

	FORCEINLINE int32 GetGoldAmount() { return GoldAmount; }

	FORCEINLINE int32 GetSoulsAmount() { return SoulsAmount; }

};
