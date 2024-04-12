// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/AttributesComponent.h"

UAttributesComponent::UAttributesComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UAttributesComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

bool UAttributesComponent::IsAlive()
{
	return Health > 0.f;
}

void UAttributesComponent::ReceiveDamage(float Damage)
{
	Health = FMath::Clamp(Health - Damage, 0.f, MaxHealth);
}

void UAttributesComponent::ConsumeStamina(float StaminaConsumption)
{
	Stamina = FMath::Clamp(Stamina - StaminaConsumption, 0.f, MaxStamina);
}

void UAttributesComponent::RecoverStamina(float StaminaRecover)
{
	Stamina = FMath::Clamp(Stamina + StaminaRecover, 0.f, MaxStamina);
}

void UAttributesComponent::AddGold(int32 InGoldAmount)
{
	GoldAmount += InGoldAmount;
}

void UAttributesComponent::AddSouls(int32 InSoulsAmount)
{
	SoulsAmount += InSoulsAmount;
}
