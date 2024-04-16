// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Soul.generated.h"

/**
 * 
 */
UCLASS()
class SLASH_API ASoul : public AItem
{
	GENERATED_BODY()

public:

	ASoul();

	virtual void BeginPlay() override;

protected:

	void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	void Destroy();

	int32 SoulAmount;

	/** Duration */

	FTimerHandle LifespanTimer;

	UPROPERTY(EditDefaultsOnly, Category = Duration)
	float Lifespan = 30.f;

public:

	/** Getters & Setters */

	FORCEINLINE int32 GetSoulAmount() { return SoulAmount; }

	FORCEINLINE void SetSoulsAmount(const int32 InSoulAmount) { SoulAmount = InSoulAmount; }

};
