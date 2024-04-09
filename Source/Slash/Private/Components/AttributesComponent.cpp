// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/AttributesComponent.h"

UAttributesComponent::UAttributesComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UAttributesComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UAttributesComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

bool UAttributesComponent::IsAlive()
{
	return Health > 0.f;
}

void UAttributesComponent::ReceiveDamage(float damage)
{
	Health = FMath::Clamp(Health - damage, 0.f, MaxHealth);
}

void UAttributesComponent::AddGold(int32 InGoldAmount)
{
	GoldAmount += InGoldAmount;
}

void UAttributesComponent::AddSouls(int32 InSoulsAmount)
{
	SoulsAmount += InSoulsAmount;
}
