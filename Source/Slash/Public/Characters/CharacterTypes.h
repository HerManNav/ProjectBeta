#pragma once

// What's the character equipping (weapon, objects, etc.)
UENUM(BlueprintType)
enum class EEquipState : uint8
{
	EES_Unequipped UMETA(DisplayName = "Unequipped"),
	EES_Disarmed UMETA(DisplayName = "Disarmed"),
	EES_ArmedOneHandedWeapon UMETA(DisplayName = "Armed one-handed weapon")
};

// What's the character doing (attacking, reacting to a hit)
UENUM(BlueprintType)
enum class EActionState : uint8
{
	EAS_Unoccupied UMETA(DisplayName = "Unoccupied"),
	EAS_ArmingDisarming UMETA(DisplayName = "ArmingDisarming"),
	EAS_HitReacting UMETA(DisplayName = "HitReacting"),
	EAS_Dodging UMETA(DisplayName = "Dodging"),
	EAS_Attacking UMETA(DisplayName = "Attacking"),
	EAS_Dead UMETA(DisplayName = "Dead")
};

// Used for Enemies only
UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	EES_Dead UMETA(DisplayName = "Dead"),
	EES_Terminal UMETA(DisplayName = "Terminal"),
	EES_Patrolling UMETA(DisplayName = "Patrolling"),
	EES_Chasing UMETA(DisplayName = "Chasing"),
	EES_Attacking UMETA(DisplayName = "Attacking"),
	EES_Engaged UMETA(DisplayName = "Engaged"),

	EES_HitReacting UMETA(DisplayName = "HitReacting"),
	EES_NoState UMETA(DisplayName = "NoState")
};

// How the character died
UENUM(BlueprintType)
enum EDeathPose
{
	EDP_Dead1 UMETA(DisplayName = "Dead1"),
	EDP_Dead2 UMETA(DisplayName = "Dead2"),
	EDP_Dead3 UMETA(DisplayName = "Dead3"),

	EDP_MAX UMETA(DisplayName = "MAX")
};