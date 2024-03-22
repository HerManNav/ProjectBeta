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

	UPROPERTY(EditAnywhere, Category = Attributes)
	float health;

	UPROPERTY(EditAnywhere, Category = Attributes)
	float maxHealth;

public:

	/*
	* Getters and setters
	*/

	float getMaxHealth() { return maxHealth; }

	bool isAlive() { return health > 0.f; }

	void takeDamage(float damage) { health = FMath::Clamp(health - damage, 0.f, maxHealth); }

	float getHealthPercent() { return health / maxHealth; }

};
