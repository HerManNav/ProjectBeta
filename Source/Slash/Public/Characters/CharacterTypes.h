#pragma once

// ENUM to record what's the character equipping (weapon, objects, etc.)
UENUM(BlueprintType)
enum class ECharacterState : uint8
{
	ECS_Unequipped UMETA(DisplayName = "Unequipped"),
	ECS_EquippedOneHandedWeapon UMETA(DisplayName = "Equipped one-handed weapon"),
	ECS_EquippedTwoHandedWeapon UMETA(DisplayName = "Equipped two-handed weapon")
};

UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	EES_Patrolling UMETA(DisplayName = "Patrolling"),
	EES_Chasing UMETA(DisplayName = "Chasing"),
	EES_Attacking UMETA(DisplayName = "Attacking")
};

// ENUM to record what's the character doing (attacking, forcing a lock)
UENUM(BlueprintType)
enum class EActionState : uint8
{
	EAS_Unoccupied UMETA(DisplayName = "Unoccupied"),
	EAS_Attacking UMETA(DisplayName =  "Attacking")
};

UENUM(BlueprintType)
enum class ELivingState : uint8
{
	ELS_Alive UMETA(DisplayName = "Alive"),
	ELS_Dead1 UMETA(DisplayName = "Dead1"),
	ELS_Dead2 UMETA(DisplayName = "Dead2"),
	ELS_Dead3 UMETA(DisplayName = "Dead3")
};