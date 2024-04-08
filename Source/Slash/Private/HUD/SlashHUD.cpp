// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/SlashHUD.h"
#include "Blueprint/UserWidget.h"
#include "HUD/SlashOverlay.h"

void ASlashHUD::BeginPlay()
{
	CreateSlashOverlayWidget();
	InitSlashOverlay();
}

void ASlashHUD::CreateSlashOverlayWidget()
{
	if (SlashOverlayClass)
	{
		SlashOverlay = CreateWidget<USlashOverlay>(GetWorld(), SlashOverlayClass);
		if (SlashOverlay)
			SlashOverlay->AddToViewport();
	}
}

void ASlashHUD::InitSlashOverlay()
{
	if (SlashOverlay)
	{
		SlashOverlay->SetHealthPercentage(1.f);
		SlashOverlay->SetStaminaPercentage(0.8f);
		SlashOverlay->SetCoinsAmount(0);
		SlashOverlay->SetSoulsAmount(0);
	}
}
