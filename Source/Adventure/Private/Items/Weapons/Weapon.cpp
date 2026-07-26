#include "Items/Weapons/Weapon.h"
#include "NiagaraSystem.h"
#include "Characters/Echo.h"
#include "NiagaraFunctionLibrary.h"
#include "DrawDebugHelpers.h"

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
	Echo = Cast<AEcho>(NewOwner);
}
void AWeapon::AttachMeshToSocket(USceneComponent* Parent, const FName& SocketName)
{
	FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget, true);
	ItemMesh->AttachToComponent(Parent, TransformRules, SocketName);
}
void AWeapon::Shoot()
{	
	if(!CanShoot()) {return;}

	if(CurrentAmmo <= 0)
	{
		StartReload();
		return;
	}
	FireTrace();
	SpawnParticle();
	ConsumeAmmo();
	

	
	//DrawDebugLine(
    //    GetWorld(), BarrelLocation, TargetPoint, FColor::Red, false, 5.f, 0, 2.f);
}
void AWeapon::StartReload()
{
	Echo->PlayReloadMontage(FName("Reload"));
	Echo->SetActionState(EActionState::EAS_Reloading);
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

    AController* Controller = Echo->GetController();
	if (!Controller) return;
	Controller->GetPlayerViewPoint(CameraLocation, CameraRotation);

	FVector CameraEnd = CameraLocation + CameraRotation.Vector() * 15000.f;

	FHitResult Hit;

	GetWorld()->LineTraceSingleByChannel(
		Hit, CameraLocation, CameraEnd, ECC_Visibility);

	FVector TargetPoint = Hit.bBlockingHit ? Hit.ImpactPoint : CameraEnd;
	FRotator BarrelRotation = (TargetPoint - BarrelLocation).Rotation();
}
bool AWeapon::CanShoot() const
{
	return Echo && Echo->GetActionState() == EActionState::EAS_Aiming;
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

