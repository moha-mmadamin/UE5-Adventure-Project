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
	if(!Parent || !ItemMesh) return;
	FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget, true);
	ItemMesh->AttachToComponent(Parent, TransformRules, SocketName);
}
bool AWeapon::TryFire()
{	
	if(!CanFire()) return false;

	FHitResult Hit;
    FireTrace(Hit);

	const FVector TargetPoint = Hit.bBlockingHit ? Hit.ImpactPoint : Hit.TraceEnd;
	const FVector ShotDirection = GetShotDirection(TargetPoint);
	const FVector BarrelLocation = ItemMesh->GetSocketLocation(TEXT("Barrel"));

	DrawDebugLine(
		GetWorld(),
		BarrelLocation,
		BarrelLocation + ShotDirection * 3000.f,
		FColor::Red,
		false,
		2.f,
		0,
		2.f
	);

	SpawnParticle();
	ConsumeAmmo();
	StartFireCooldown();

	return true;
}
void AWeapon::ReloadAmmo()
{
    if(!CanReload()) return;

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
void AWeapon::FireTrace(FHitResult& OutHit) const
{
	OutHit = FHitResult();

	APawn* OwnerPawn = Cast<APawn>(GetOwner());
    if(!OwnerPawn) return;

    AController* Controller = OwnerPawn->GetController();
	if (!Controller) return;

	FRotator CameraRotation;
	FVector CameraLocation;

	Controller->GetPlayerViewPoint(CameraLocation, CameraRotation);

	const FVector TraceEnd = CameraLocation + CameraRotation.Vector() * 15000.f;

	GetWorld()->LineTraceSingleByChannel(OutHit, CameraLocation, TraceEnd, ECC_Visibility);
}
void AWeapon::ResetFire()
{
	bCanFire = true;
}
void AWeapon::StartFireCooldown()
{
	bCanFire = false;

	GetWorld()->GetTimerManager().SetTimer(
        FireRateHandle,
        this,
        &AWeapon::ResetFire,
        FireRate,
        false
    );
}
FVector AWeapon::GetShotDirection(const FVector& TargetPoint) const
{
	const FVector BarrelLocation = ItemMesh->GetSocketLocation(TEXT("Barrel"));
	return (TargetPoint - BarrelLocation).GetSafeNormal();
}
bool AWeapon::CanFire() const
{
	return bCanFire && CurrentAmmo > 0;
}
void AWeapon::SpawnParticle() const
{
	if (!FireEffect) return;
	
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

