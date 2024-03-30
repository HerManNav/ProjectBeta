// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "HealthBarComponent.generated.h"

/**
 * 
 */
UCLASS()
class SLASH_API UHealthBarComponent : public UWidgetComponent
{
	GENERATED_BODY()
	
public:

	void SetPercentage(float Percentage);

protected:

	UPROPERTY()		// With this we make sure that healthBarWidget is null and not garbage! Unreal must take it into account for that
	TObjectPtr<class UHealthBar> HealthBarWidget;

};
