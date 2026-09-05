#pragma once

UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	EWS_Unarmed UMETA(DisplayName = "Unarmed"),
	EWS_Holstered UMETA(DisplayName = "Holstered"),
	EWS_Equipped UMETA(DisplayName = "Equipped")
};

UENUM(BlueprintType)
enum class ECharacterState : uint8
{
    ECS_Alive UMETA(DisplayName = "Alive"),
    ECS_Dead UMETA(DisplayName = "Dead")
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

UENUM(BlueprintType)
enum EDeathPose
{
	EDP_Death1 UMETA(DisplayName = "Death1"),
	EDP_Death2 UMETA(DisplayName = "Death2"),
	EDP_Death3 UMETA(DisplayName = "Death3"),
	EDP_Death4 UMETA(DisplayName = "Death4"),
	EDP_Death5 UMETA(DisplayName = "Death5"),
	EDP_Death6 UMETA(DisplayName = "Death6"),
	EDP_Death7 UMETA(DisplayName = "Death7"),

	EDP_Max UMETA(DisplayName = "DefaultMax")

};