#include "Items/Weapons/Weapon.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Controller.h"

AWeapon::AWeapon()
{

}
void AWeapon::BeginPlay()
{
	Super::BeginPlay();

}
void AWeapon::Equip(USceneComponent* Parent, const FName& SocketName, AActor* NewOwner, APawn* NewInstigator)
{
	ItemState = EItemState::EIS_Equipped;
	SetOwner(NewOwner);
	SetInstigator(NewInstigator);
	AttachMeshToSocket(Parent, SocketName);
}
void AWeapon::AttachMeshToSocket(USceneComponent* Parent, const FName& SocketName)
{
	FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget, true);
	ItemMesh->AttachToComponent(Parent, TransformRules, SocketName);
}
bool AWeapon::TryFire()
{	
	if(!CanShoot()) return false;

	FireTrace();
	SpawnParticle();
	ConsumeAmmo();
	StartFireCooldown();

	return true;
}
void AWeapon::ReloadAmmo()
{
	if (CurrentAmmo >= MagazineCapacity) return;
	if(ReserveAmmo <= 0) return;

	const int32 NeededAmmo = MagazineCapacity - CurrentAmmo;
	const int32 AmmoToLoad = FMath::Min(NeededAmmo, ReserveAmmo);

	CurrentAmmo += AmmoToLoad;
	ReserveAmmo -= AmmoToLoad;

	OnAmmoChanged.Broadcast(CurrentAmmo, ReserveAmmo);
}
bool AWeapon::CanReload() const
{
	return CurrentAmmo < MagazineCapacity && ReserveAmmo > 0;
}
void AWeapon::ConsumeAmmo()
{
	CurrentAmmo--;
	OnAmmoChanged.Broadcast(CurrentAmmo, ReserveAmmo);
}
void AWeapon::FireTrace()
{
	FVector BarrelLocation = ItemMesh->GetSocketLocation(TEXT("Barrel"));

	FRotator CameraRotation;
	FVector CameraLocation;

	APawn* OwnerPawn = Cast<APawn>(GetOwner());
    if(!OwnerPawn) return;

    AController* Controller = OwnerPawn->GetController();
	if (!Controller) return;

	Controller->GetPlayerViewPoint(CameraLocation, CameraRotation);

	FVector CameraEnd = CameraLocation + CameraRotation.Vector() * 15000.f;

	FHitResult Hit;

	GetWorld()->LineTraceSingleByChannel(
		Hit, CameraLocation, CameraEnd, ECC_Visibility);

	FVector TargetPoint = Hit.bBlockingHit ? Hit.ImpactPoint : CameraEnd;
	FRotator BarrelRotation = (TargetPoint - BarrelLocation).Rotation();

	//DrawDebugLine(
    //    GetWorld(), BarrelLocation, TargetPoint, FColor::Red, false, 5.f, 0, 2.f);
}
void AWeapon::ResetFire()
{
	CanFire = true;
}
void AWeapon::StartFireCooldown()
{
	CanFire = false;

	GetWorld()->GetTimerManager().SetTimer(
        FireRateHandle,
        this,
        &AWeapon::ResetFire,
        FireRate,
        false
    );
}
bool AWeapon::CanShoot() const
{
	return CanFire && CurrentAmmo > 0;
}
void AWeapon::SpawnParticle()
{
	if (FireEffect && ItemMesh)
	{
		const FVector Location =
            ItemMesh->GetSocketLocation(FName("Barrel"));

        const FRotator Rotation =
            ItemMesh->GetSocketRotation(FName("Barrel"));

		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
        GetWorld(),
        FireEffect,
        Location,
        Rotation
		);
	}
}

