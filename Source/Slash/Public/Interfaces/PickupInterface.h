// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PickupInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPickupInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SLASH_API IPickupInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent)
	void SetOverlappingItem(AItem* Item);

	UFUNCTION(BlueprintNativeEvent)
	void PickupTreasure(AItem* Item);

	UFUNCTION(BlueprintNativeEvent)
	void PickupSoul(AItem* Item);

};
