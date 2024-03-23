#pragma once

UENUM(BlueprintType)
enum class ELivingState : uint8
{
	ELS_Alive UMETA(DisplayName = "Alive"),
	ELS_Dead1 UMETA(DisplayName = "Dead1"),
	ELS_Dead2 UMETA(DisplayName = "Dead2"),
	ELS_Dead3 UMETA(DisplayName = "Dead3")
};

UENUM(BlueprintType)
enum class ECombatState : uint8
{
	ECS_Patrolling UMETA(DisplayName = "Patrolling"),
	ECS_Chasing UMETA(DisplayName = "Chasing"),
	ECS_Attacking UMETA(DisplayName = "Attacking"),
	ECS_Dead UMETA(DisplayName = "Dead"),
};