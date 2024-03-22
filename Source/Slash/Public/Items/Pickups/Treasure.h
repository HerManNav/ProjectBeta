// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Treasure.generated.h"

class USoundBase;

/**
 * 
 */
UCLASS()
class SLASH_API ATreasure : public AItem
{
	GENERATED_BODY()

public:

	ATreasure();

protected:

	UPROPERTY(EditAnywhere, Category = Sound)
	TObjectPtr<USoundBase> pickupSound;

	void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Treasure properties")
	int32 goldValue;

};
