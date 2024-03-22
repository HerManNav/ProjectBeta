#pragma once

UENUM(BlueprintType)
enum class ELivingState : uint8
{
	ELS_Alive UMETA(DisplayName = "Alive"),
	ELS_Dead1 UMETA(DisplayName = "Dead1"),
	ELS_Dead2 UMETA(DisplayName = "Dead2"),
	ELS_Dead3 UMETA(DisplayName = "Dead3")
};