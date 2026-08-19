#include "Characters/BaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Items/Weapons/Weapon.h"

ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

}
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
    if(bSpawnDefaultWeapon)
    {
        SpawnDefaultWeapon();
    }
}
void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}
void ABaseCharacter::PlayReloadMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
    if(AnimInstance && ReloadMontage)
    {
        AnimInstance->Montage_Play(ReloadMontage);
        AnimInstance->Montage_JumpToSection(SectionName, ReloadMontage);
    }
}
void ABaseCharacter::Reload()
{
    if(!EquippedWeapon || !EquippedWeapon->CanReload()) return;

	PlayReloadMontage(FName("Reload"));
}
void ABaseCharacter::PlayEquipMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if(AnimInstance && EquipMontage)
	{
		AnimInstance->Montage_Play(EquipMontage);
		AnimInstance->Montage_JumpToSection(SectionName, EquipMontage);
	}
}
void ABaseCharacter::PlayFireMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
    if(AnimInstance && FireMontage)
    {
        AnimInstance->Montage_Play(FireMontage);
        AnimInstance->Montage_JumpToSection(SectionName, FireMontage);
    }
}
void ABaseCharacter::Fire()
{
    if(EquippedWeapon && EquippedWeapon->TryFire())
    {
        PlayFireMontage(FName("Fire"));
    } 
}
void ABaseCharacter::Aim()
{
    if(!CanAim()) return;

    CombatState = ECombatState::ECS_Aiming;
}
void ABaseCharacter::StopAiming()
{
    CombatState = ECombatState::ECS_Idle;
}
void ABaseCharacter::SetMovementSpeed(float Speed)
{
    GetCharacterMovement()->MaxWalkSpeed = Speed;
}
void ABaseCharacter::SpawnDefaultWeapon()
{
    UWorld* World = GetWorld();
    if(!World || !WeaponClass) return;

    AWeapon* DefaultWeapon = World->SpawnActor<AWeapon>(WeaponClass);
    if(DefaultWeapon)
    {
        DefaultWeapon->Equip(GetMesh(), WeaponHolsterSocket, this, this);
        EquippedWeapon = DefaultWeapon;
        WeaponState = EWeaponState::EWS_Unarmed;
    }
}
bool ABaseCharacter::CanArm()
{
	return false;
}
bool ABaseCharacter::CanDisarm()
{
	return false;
}
bool ABaseCharacter::CanReload() const
{
    return EquippedWeapon && EquippedWeapon->CanReload();
}
bool ABaseCharacter::CanFire() const
{
    return EquippedWeapon && 
        EquippedWeapon->CanFire();
}
bool ABaseCharacter::CanAim() const
{
    return EquippedWeapon &&
        CombatState == ECombatState::ECS_Idle;
}
void ABaseCharacter::EquipWeapon(AWeapon* Weapon)
{
    if(!Weapon) return;
    Weapon->Equip(GetMesh(), WeaponHandSocket, this, this);
    OverlappingItem = nullptr;
    EquippedWeapon = Weapon;
    WeaponState = EWeaponState::EWS_Equipped;
}
void ABaseCharacter::Arm()
{
    if(EquippedWeapon)
    {
        EquippedWeapon->AttachMeshToSocket(GetMesh(), WeaponHandSocket);
    }
}
void ABaseCharacter::Disarm()
{
	if(EquippedWeapon)
    {
        EquippedWeapon->AttachMeshToSocket(GetMesh(), WeaponHolsterSocket);
    }
}
void ABaseCharacter::FinishWeaponEquip_Implementation()
{
    if(EquippedWeapon)
    {
        EquippedWeapon->AttachMeshToSocket(GetMesh(), WeaponHandSocket);
    }
    WeaponState = EWeaponState::EWS_Equipped;
    CombatState = ECombatState::ECS_Idle;
}
void ABaseCharacter::FinishWeaponUnequip_Implementation()
{
    if(EquippedWeapon)
    {
        EquippedWeapon->AttachMeshToSocket(GetMesh(), WeaponHolsterSocket);
    }
    WeaponState = EWeaponState::EWS_Unarmed;
    CombatState = ECombatState::ECS_Idle;
}
void ABaseCharacter::FinishReloading_Implementation()
{
    if(EquippedWeapon)
    {
        EquippedWeapon->ReloadAmmo();
    }
}