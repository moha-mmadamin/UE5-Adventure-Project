#pragma once

UENUM(BlueprintType)
enum class ECharacterState : uint8
{
	ECS_Unequipped UMETA(DisplayName = "Unequipped"),
	ECS_EquippedGun UMETA(DisplayName = "Equipped Gun")
};

UENUM(BlueprintType)
enum class EActionState : uint8
{
	EAS_Unoccupied UMETA(DisplayName = "Unoccupied"),
	EAS_Shooting UMETA(DisplayName = "Shooting"),
	EAS_EquippingWeapon UMETA(DisplayName = "Equipping weapon"),
	EAS_Aiming UMETA(DisplayName = "Aiming"),
	EAS_Reloading UMETA(DisplayName = "Reloading")
};
