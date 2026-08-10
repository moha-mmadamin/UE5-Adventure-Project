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
	SpawnDefaultWeapon();
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
    IsAiming = true;
}
void ABaseCharacter::StopAiming()
{
    IsAiming = false;
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
        DefaultWeapon->Equip(GetMesh(), FName("PistolSocket"), this, this);
        EquippedWeapon = DefaultWeapon;
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
    return EquippedWeapon != nullptr;
}
void ABaseCharacter::EquipWeapon(AWeapon* Weapon)
{
    Weapon->Equip(GetMesh(), FName("RightHandSocket"), this, this);
    OverlappingItem = nullptr;
    EquippedWeapon = Weapon;
}
void ABaseCharacter::Arm()
{
    if(EquippedWeapon)
    {
        EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("RightHandSocket"));
    }
}
void ABaseCharacter::Disarm()
{
	if(EquippedWeapon)
    {
        EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("PistolSocket"));
    }
}
void ABaseCharacter::FinishEquipping_Implementation()
{
}
void ABaseCharacter::FinishReloading_Implementation()
{
    if(EquippedWeapon)
    {
        EquippedWeapon->ReloadAmmo();
    }
}