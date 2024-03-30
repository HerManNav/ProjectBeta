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
	float health = 100.f;

	UPROPERTY(EditAnywhere, Category = Attributes, meta = (ClampMin = "0"))
	float maxHealth = 100.f;

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

	float getMaxHealth() { return maxHealth; }

	bool isAlive() { return health > 0.f; }

	void takeDamage(float damage) { health = FMath::Clamp(health - damage, 0.f, maxHealth); }

	float getHealthPercent() { return health / maxHealth; }

	float GetWalkingSpeedUnequipped() { return WalkingSpeedUnequipped; }

	float GetRunningSpeedUnequipped() { return RunningSpeedUnequipped; }

	float GetWalkingSpeedEquipped() { return WalkingSpeedEquipped; }

	float GetRunningSpeedEquipped() { return RunningSpeedEquipped; }

};
