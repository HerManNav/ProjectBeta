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

	void PlayPickupParticles();

	int32 SoulAmount;

	UPROPERTY(EditAnywhere)
	int32 MinSouls = 5;

	UPROPERTY(EditAnywhere)
	int32 MaxSouls = 10;

public:

	FORCEINLINE int32 GetSoulAmount() { return SoulAmount; }
};
