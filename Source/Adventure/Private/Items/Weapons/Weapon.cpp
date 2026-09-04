#include "Items/Weapons/Weapon.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Controller.h"
#include "Components/Health/HealthComponent.h"

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

	FHitResult CameraHit;
    FireTrace(CameraHit);

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

	if(ShotHit.bBlockingHit)
	{
		AActor* HitActor = ShotHit.GetActor();
		if(HitActor)
		{
			UHealthComponent* HealthComponent = HitActor->FindComponentByClass<UHealthComponent>();
			if(HealthComponent)
			{
                const float Damage = GetDamageForBone(ShotHit.BoneName);
                UE_LOG(
                    LogTemp,
                    Warning,
                    TEXT("Hit Actor: %s | Bone: %s | Damage: %.1f"),
                    *GetNameSafe(HitActor),
                    *ShotHit.BoneName.ToString(),
                    Damage
                );
				HealthComponent->TakeDamage(Damage);
			}
		}
	}

	//DrawDebugLine(
	//	GetWorld(),
	//	BarrelLocation,
 //       ImpactPoint,
	//	FColor::Red,
	//	false,
	//	2.f,
	//	0,
	//	2.f
	//);

	SpawnParticle();
	ConsumeAmmo();
	StartFireCooldown();

	return true;
}
void AWeapon::ReloadAmmo()
{
    if(!CanReloadAmmo()) return;

	const int32 NeededAmmo = MagazineCapacity - CurrentAmmo;
	const int32 AmmoToLoad = FMath::Min(NeededAmmo, ReserveAmmo);

	CurrentAmmo += AmmoToLoad;
	ReserveAmmo -= AmmoToLoad;

	OnAmmoChanged.Broadcast(CurrentAmmo, ReserveAmmo);
}
bool AWeapon::CanReloadAmmo() const
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
float AWeapon::GetDamageForBone(FName BoneName) const
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

