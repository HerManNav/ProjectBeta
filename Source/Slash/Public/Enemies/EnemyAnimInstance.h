// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "EnemyAnimInstance.generated.h"

class AEnemy;

/**
 * 
 */
UCLASS()
class SLASH_API UEnemyAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:

	void NativeInitializeAnimation() override;
	
	void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;

	UPROPERTY()
	TObjectPtr<AEnemy> Enemy;

	UPROPERTY(BlueprintReadWrite, Category = Movement)
	float GroundSpeed;

};
