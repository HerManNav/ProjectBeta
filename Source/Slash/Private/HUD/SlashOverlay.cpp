// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/SlashOverlay.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void USlashOverlay::SetHealthPercentage(float HealthFraction)
{
	if (HealthBar)
		HealthBar->SetPercent(HealthFraction);
}

void USlashOverlay::SetStaminaPercentage(float StaminaFraction)
{
	if (StaminaBar)
		StaminaBar->SetPercent(StaminaFraction);
}

void USlashOverlay::SetCoinsAmount(int16 Coins)
{
	if (CoinsAmountText)
	{
		const FString CoinsText = FString::FromInt(Coins);
		CoinsAmountText->SetText(FText::FromString(CoinsText));
	}

}

void USlashOverlay::SetSoulsAmount(int16 Souls)
{
	if (SoulsAmountText)
	{
		const FString SoulsText = FString::FromInt(Souls);
		SoulsAmountText->SetText(FText::FromString(SoulsText));
	}
}
