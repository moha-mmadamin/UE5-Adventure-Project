#pragma once

UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	EWS_Unarmed UMETA(DisplayName = "Unarmed"),
	EWS_Holstered UMETA(DisplayName = "Holstered"),
	EWS_Equipped UMETA(DisplayName = "Equipped")
};

UENUM(BlueprintType)
enum class ECombatState : uint8
{
	ECS_Idle UMETA(DisplayName = "Idle"),
	ECS_EquippingWeapon UMETA(DisplayName = "Equipping Weapon"),
	ECS_Aiming UMETA(DisplayName = "Aiming"),
	ECS_Firing UMETA(DisplayName= "Firing"),
	ECS_UnequippingWeapon UMETA(DisplayName = "Unequipping Weapon"),
	ECS_Reloading UMETA(DisplayName = "Reloading")
};
