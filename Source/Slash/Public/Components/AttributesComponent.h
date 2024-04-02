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

protected:

	virtual void BeginPlay() override;

private:

	UPROPERTY(EditAnywhere, Category = Attributes, meta = (ClampMin = "0"))
	float Health = 100.f;

	UPROPERTY(EditAnywhere, Category = Attributes, meta = (ClampMin = "0"))
	float MaxHealth = 100.f;

	UPROPERTY(EditAnywhere, Category = Movement)
	float WalkingSpeedUnequipped = 200.f;

	UPROPERTY(EditAnywhere, Category = Movement)
	float RunningSpeedUnequipped = 400.f;

	UPROPERTY(EditAnywhere, Category = Movement)
	float WalkingSpeedEquipped = 125.f;

	UPROPERTY(EditAnywhere, Category = Movement)
	float RunningSpeedEquipped = 300.f;

public:

	/*
	* Getters and setters
	*/

	float GetMaxHealth() { return MaxHealth; }

	bool IsAlive() { return Health > 0.f; }

	void ReceiveDamage(float damage) { Health = FMath::Clamp(Health - damage, 0.f, MaxHealth); }

	float getHealthPercent() { return Health / MaxHealth; }

	float GetWalkingSpeedUnequipped() { return WalkingSpeedUnequipped; }

	float GetRunningSpeedUnequipped() { return RunningSpeedUnequipped; }

	float GetWalkingSpeedEquipped() { return WalkingSpeedEquipped; }

	float GetRunningSpeedEquipped() { return RunningSpeedEquipped; }

};
