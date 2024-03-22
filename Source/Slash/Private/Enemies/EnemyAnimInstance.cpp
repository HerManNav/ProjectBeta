// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/EnemyAnimInstance.h"
#include "Enemies/Enemy.h"

void UEnemyAnimInstance::NativeInitializeAnimation()
{
	enemy = Cast<AEnemy>(TryGetPawnOwner());
}

void UEnemyAnimInstance::NativeThreadSafeUpdateAnimation(float deltaSeconds)
{
	if (enemy)
		groundSpeed = enemy->GetVelocity().Size2D();
}
