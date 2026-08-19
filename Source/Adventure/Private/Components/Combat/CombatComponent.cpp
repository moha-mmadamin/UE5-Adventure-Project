#include "Components/Combat/CombatComponent.h"
#include "Characters/BaseCharacter.h"
#include "Items/Weapons/Weapon.h"

UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}
void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

    Character = Cast<ABaseCharacter>(GetOwner());

    if(bSpawnDefaultWeapon)
    {
        SpawnDefaultWeapon();
    }
}
void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}
void UCombatComponent::Fire()
{
	if(!CanFire()) return;
    if(EquippedWeapon && EquippedWeapon->TryFire())
    {
        Character->PlayFireMontage(FName("Fire"));
    } 
}
void UCombatComponent::Reload()
{
    if(!CanReload()) return;
    if(!EquippedWeapon || !EquippedWeapon->CanReload()) return;

	Character->PlayReloadMontage(FName("Reload"));
    CombatState = ECombatState::ECS_Reloading;
}
void UCombatComponent::StartAiming()
{
    if(!CanAim()) return;

    CombatState = ECombatState::ECS_Aiming;
}
void UCombatComponent::StopAiming()
{
    CombatState = ECombatState::ECS_Idle;
}
void UCombatComponent::EquipWeapon(AWeapon* Weapon)
{
    if(!Weapon || !Character) return;
    Weapon->Equip(Character->GetMesh(), WeaponHandSocket, Character, Character);
    Character->SetOverlappingItem(nullptr);
    EquippedWeapon = Weapon;
    WeaponState = EWeaponState::EWS_Equipped;
}
bool UCombatComponent::CanReload() const
{
    return EquippedWeapon && 
        EquippedWeapon->CanReload() &&
        WeaponState == EWeaponState::EWS_Equipped;
}
bool UCombatComponent::CanFire() const
{
    return EquippedWeapon && 
        EquippedWeapon->CanFire() &&
        WeaponState == EWeaponState::EWS_Equipped &&
        CombatState == ECombatState::ECS_Aiming;
}
bool UCombatComponent::CanAim() const
{
    return EquippedWeapon &&
        CombatState == ECombatState::ECS_Idle;
}
bool UCombatComponent::CanArm() const
{
    return CombatState == ECombatState::ECS_Idle &&
        WeaponState == EWeaponState::EWS_Unarmed && 
        EquippedWeapon;
}
bool UCombatComponent::CanDisarm() const
{
    return CombatState == ECombatState::ECS_Idle &&
        WeaponState != EWeaponState::EWS_Unarmed;
}
void UCombatComponent::SpawnDefaultWeapon()
{
    UWorld* World = GetWorld();
    if(!World || !WeaponClass) return;

    AWeapon* DefaultWeapon = World->SpawnActor<AWeapon>(WeaponClass);
    if(DefaultWeapon)
    {
        DefaultWeapon->Equip(Character->GetMesh(), WeaponHolsterSocket, Character, Character);
        EquippedWeapon = DefaultWeapon;
        WeaponState = EWeaponState::EWS_Unarmed;
    }
}
void UCombatComponent::Arm()
{
    if(EquippedWeapon)
    {
        EquippedWeapon->AttachMeshToSocket(Character->GetMesh(), WeaponHandSocket);
    }
}
void UCombatComponent::Disarm()
{
	if(EquippedWeapon)
    {
        EquippedWeapon->AttachMeshToSocket(Character->GetMesh(), WeaponHolsterSocket);
    }
}
void UCombatComponent::FinishWeaponEquip_Implementation()
{
    if(EquippedWeapon)
    {
        EquippedWeapon->AttachMeshToSocket(Character->GetMesh(), WeaponHandSocket);
    }
    WeaponState = EWeaponState::EWS_Equipped;
    CombatState = ECombatState::ECS_Idle;
}
void UCombatComponent::FinishWeaponUnequip_Implementation()
{
    if(EquippedWeapon)
    {
        EquippedWeapon->AttachMeshToSocket(Character->GetMesh(), WeaponHolsterSocket);
    }
    WeaponState = EWeaponState::EWS_Unarmed;
    CombatState = ECombatState::ECS_Idle;
}
void UCombatComponent::FinishReloading_Implementation()
{
    if(EquippedWeapon)
    {
        EquippedWeapon->ReloadAmmo();
    }
    CombatState = ECombatState::ECS_Idle;
}