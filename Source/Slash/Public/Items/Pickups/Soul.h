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
	virtual void Tick(float DeltaTime) override;

protected:

	void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	float GetTargetZ();

	int32 SoulAmount;

	float TargetZ;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Soul properties|Drift")
	float DesiredTargetAltitude = 75.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Soul properties|Drift")
	float DriftingSpeed = 30.f;

	UPROPERTY(EditDefaultsOnly, Category = "Soul properties")
	float Lifespan = 30.f;

	UPROPERTY(EditDefaultsOnly, Category = "Soul properties|Debug")
	bool bShowDebugTrace = false;

public:

	/** Getters & Setters */

	FORCEINLINE int32 GetSoulAmount() { return SoulAmount; }

	FORCEINLINE void SetSoulsAmount(const int32 InSoulAmount) { SoulAmount = InSoulAmount; }

};
