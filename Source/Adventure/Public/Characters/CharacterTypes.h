#pragma once

UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	EWS_Unarmed UMETA(DisplayName = "Unarmed"),
	EWS_Equipped UMETA(DisplayName = "Equipped")
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
