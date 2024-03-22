// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/HealthBarComponent.h"
#include "HUD/HealthBar.h"

void UHealthBarComponent::setPercentage(float percentage)
{
	if (!healthBarWidget)	// We do this only the first time (when it's null, because casting every time is expensive! So we save it as a member variable
		healthBarWidget = Cast<UHealthBar>(GetUserWidgetObject());

	if (healthBarWidget)
		healthBarWidget->setPercentage(percentage);
}
