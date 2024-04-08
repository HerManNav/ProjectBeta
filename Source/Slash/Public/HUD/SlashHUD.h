// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "SlashHUD.generated.h"

class USlashOverlay;

/**
 * 
 */
UCLASS()
class SLASH_API ASlashHUD : public AHUD
{
	GENERATED_BODY()

protected:

	virtual void BeginPlay() override;

	void CreateSlashOverlayWidget();
	void InitSlashOverlay();

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> SlashOverlayClass;

	UPROPERTY()
	TObjectPtr<USlashOverlay> SlashOverlay;

public:

	FORCEINLINE USlashOverlay* GetSlashOverlay() { return SlashOverlay;  }
};
