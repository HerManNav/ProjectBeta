// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/EnemyAnimInstance.h"
#include "Enemies/Enemy.h"

void UEnemyAnimInstance::NativeInitializeAnimation()
{
	Enemy = Cast<AEnemy>(TryGetPawnOwner());
}

void UEnemyAnimInstance::NativeThreadSafeUpdateAnimation(float deltaSeconds)
{
	if (Enemy)
		GroundSpeed = Enemy->GetVelocity().Size2D();
}
