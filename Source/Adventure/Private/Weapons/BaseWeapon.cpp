#include "Weapons/BaseWeapon.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Health/HealthComponent.h"

ABaseWeapon::ABaseWeapon()
{
}
void ABaseWeapon::BeginPlay()
{
}
void ABaseWeapon::ConsumeAmmo()
{
	CurrentAmmo--;
	OnAmmoChanged.Broadcast(CurrentAmmo, ReserveAmmo);
}
void ABaseWeapon::SpawnParticle() const
{
	if (!FireEffect) return;
	
	const FVector Location = ItemMesh->GetSocketLocation(FName("Barrel"));
    const FRotator Rotation = ItemMesh->GetSocketRotation(FName("Barrel"));

	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), FireEffect, Location, Rotation);
}
void ABaseWeapon::SpawnBlood(const FHitResult& HitResult)
{
    if(!HitResult.bBlockingHit) return;
    AActor* HitActor = HitResult.GetActor();
    if(!HitActor) return;
    UHealthComponent* HealthComponent = HitActor->FindComponentByClass<UHealthComponent>();
    if(!HealthComponent) return;
    if(!BloodParticle) return;

    const FVector SpawnLocation = HitResult.ImpactPoint;
    FRotator SpawnRotation = HitResult.ImpactNormal.Rotation();
    SpawnRotation.Pitch -= 90.0f;
    const FVector SpawnScale(0.4f);

    UGameplayStatics::SpawnEmitterAtLocation(
        GetWorld(),
        BloodParticle,
        SpawnLocation,
        SpawnRotation,
        SpawnScale
    );
}
void ABaseWeapon::ApplyDamage(const FHitResult& HitResult)
{
    if(!HitResult.bBlockingHit) return;

    AActor* HitActor = HitResult.GetActor();
    if(!HitActor) return;

    UHealthComponent* HealthComponent = HitActor->FindComponentByClass<UHealthComponent>();
    if(!HealthComponent) return;

    const float Damage = GetDamageForBone(HitResult.BoneName);

    UE_LOG(LogTemp, Warning, TEXT("DAMAGE: %.1f -> %s"), Damage, *GetNameSafe(HitActor));

    HealthComponent->TakeDamage(Damage);
}
//FVector ABaseWeapon::GetShotDirection(const FVector& TargetPoint) const
//{
//	const FVector BarrelLocation = ItemMesh->GetSocketLocation(TEXT("Barrel"));
//	return (TargetPoint - BarrelLocation).GetSafeNormal();
//}
void ABaseWeapon::StartFireCooldown()
{
	bCanFire = false;

	GetWorld()->GetTimerManager().SetTimer(
        FireRateHandle,
        this,
        &ABaseWeapon::ResetFire,
        FireRate,
        false
    );
}
void ABaseWeapon::ResetFire()
{
	bCanFire = true;
}
void ABaseWeapon::Equip(USceneComponent* Parent, const FName& SocketName, AActor* NewOwner, APawn* NewInstigator)
{
	SetOwner(NewOwner);
	SetInstigator(NewInstigator);
	AttachMeshToSocket(Parent, SocketName);
}
void ABaseWeapon::AttachMeshToSocket(USceneComponent* Parent, const FName& SocketName)
{
	if(!Parent || !ItemMesh) return;
	FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget, true);
	ItemMesh->AttachToComponent(Parent, TransformRules, SocketName);
}
bool ABaseWeapon::Fire()
{
	if(!CanFire()) return false;

	FHitResult CameraHit;
    GetCameraTrace(CameraHit);

    const FVector TargetPoint = CameraHit.bBlockingHit ? CameraHit.ImpactPoint : CameraHit.TraceEnd;

	const FVector BarrelLocation = ItemMesh->GetSocketLocation(TEXT("Barrel"));
	const FVector ShotDirection = (TargetPoint - BarrelLocation).GetSafeNormal();
	const FVector ShotEnd = BarrelLocation + ShotDirection * 15000.f;

    FHitResult ShotHit;

    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(this);
    QueryParams.AddIgnoredActor(GetOwner());

    GetWorld()->LineTraceSingleByChannel(
        ShotHit,
        BarrelLocation,
        ShotEnd,
        ECC_Visibility,
        QueryParams
    );

    const FVector ImpactPoint = ShotHit.bBlockingHit ? ShotHit.ImpactPoint : ShotEnd;

	//DrawDebugLine(
	//	GetWorld(),
	//	BarrelLocation,
    //    ImpactPoint,
	//	FColor::Red,
	//	false,
	//	2.f,
	//	0,
	//	2.f
	//);

    ApplyDamage(ShotHit);
    SpawnBlood(ShotHit);
	SpawnParticle();
	ConsumeAmmo();
	StartFireCooldown();

	return true;
}
void ABaseWeapon::GetCameraTrace(FHitResult& OutHit) const
{
	OutHit = FHitResult();

	APawn* OwnerPawn = Cast<APawn>(GetOwner());
    if(!OwnerPawn) return;

    AController* Controller = OwnerPawn->GetController();
	if (!Controller) return;

	FRotator CameraRotation;
	FVector CameraLocation;

	Controller->GetPlayerViewPoint(CameraLocation, CameraRotation);

	const FVector TraceDirection = CameraRotation.Vector();
	const FVector TraceEnd = CameraLocation + TraceDirection * 15000.f;

    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(OwnerPawn);
    QueryParams.AddIgnoredActor(this);

    GetWorld()->LineTraceSingleByChannel(
        OutHit,
        CameraLocation,
        TraceEnd,
        ECC_Visibility,
        QueryParams
    );
    const FVector DebugEnd = OutHit.bBlockingHit ? OutHit.ImpactPoint : TraceEnd;


    //DrawDebugLine(
    //    GetWorld(),
    //    CameraLocation,
    //    DebugEnd,
    //    FColor::Green,
    //    false,
    //    2.f,
    //    0,
    //    1.f
    //);
}
void ABaseWeapon::ReloadAmmo()
{
    if(!CanReloadAmmo()) return;

	const int32 NeededAmmo = MagazineCapacity - CurrentAmmo;
	const int32 AmmoToLoad = FMath::Min(NeededAmmo, ReserveAmmo);

	CurrentAmmo += AmmoToLoad;
	ReserveAmmo -= AmmoToLoad;

	OnAmmoChanged.Broadcast(CurrentAmmo, ReserveAmmo);
}
bool ABaseWeapon::CanReloadAmmo() const
{
	return CurrentAmmo < MagazineCapacity && ReserveAmmo > 0;
}
bool ABaseWeapon::CanFire() const
{
	return bCanFire && CurrentAmmo > 0;
}
float ABaseWeapon::GetDamageForBone(FName BoneName) const
{
	if(BoneName == TEXT("head"))
	{
		return FMath::RandRange(35.f, 50.f);
	}
    if (BoneName == TEXT("spine_01") ||
        BoneName == TEXT("spine_02") ||
        BoneName == TEXT("spine_03") ||
        BoneName == TEXT("spine_04") ||
        BoneName == TEXT("spine_05"))
    {
        return FMath::RandRange(15.f, 30.f);
    }

    if (BoneName == TEXT("upperarm_l") ||
        BoneName == TEXT("upperarm_r") ||
        BoneName == TEXT("lowerarm_l") ||
        BoneName == TEXT("lowerarm_r") ||
        BoneName == TEXT("hand_l") ||
        BoneName == TEXT("hand_r"))
    {
        return FMath::RandRange(5.f, 10.f);
    }

    if (BoneName == TEXT("thigh_l") ||
        BoneName == TEXT("thigh_r") ||
        BoneName == TEXT("calf_l") ||
        BoneName == TEXT("calf_r") ||
        BoneName == TEXT("foot_l") ||
        BoneName == TEXT("foot_r"))
    {
        return FMath::RandRange(5.f, 10.f);
    }
    return 15.f;
}