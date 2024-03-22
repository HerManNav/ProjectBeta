#pragma once

// ENUM to record what's the character equipping (weapon, objects, etc.)
UENUM(BlueprintType)
enum class ECharacterState : uint8
{
	ECS_Unequipped UMETA(DisplayName = "Unequipped"),
	ECS_EquippedOneHandedWeapon UMETA(DisplayName = "Equipped one-handed weapon"),
	ECS_EquippedTwoHandedWeapon UMETA(DisplayName = "Equipped two-handed weapon")
};

// ENUM to record what's the character doing (attacking, forcing a lock)
UENUM(BlueprintType)
enum class EActionState : uint8
{
	EAS_Unoccupied UMETA(DisplayName = "Unoccupied"),
	EAS_Attacking UMETA(DisplayName =  "Attacking")
};