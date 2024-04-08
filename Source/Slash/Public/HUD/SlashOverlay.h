// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SlashOverlay.generated.h"

class UProgressBar;
class UTextBlock;

/**
 * 
 */
UCLASS()
class SLASH_API USlashOverlay : public UUserWidget
{
	GENERATED_BODY()
	

private:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> HealthBar;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> StaminaBar;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> CoinsAmountText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> SoulsAmountText;

public:

	void SetHealthPercentage(float HealthFraction);

	void SetStaminaPercentage(float StaminaFraction);

	void SetCoinsAmount(int16 Coins);

	void SetSoulsAmount(int16 Souls);

};
