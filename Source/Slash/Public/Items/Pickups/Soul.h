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

	int32 SoulValue;

	UPROPERTY(EditAnywhere)
	int32 MinValue = 5.f;

	UPROPERTY(EditAnywhere)
	int32 MaxValue = 10.f;

};
