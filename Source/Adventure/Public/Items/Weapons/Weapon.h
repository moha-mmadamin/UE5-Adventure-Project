#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "TimerManager.h"
#include "Weapon.generated.h"


class UBoxComponent;
class UNiagaraSystem;
class AEcho;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnAmmoChanged,
	int32, CurrentAmmo,
	int32, ReserveAmmo
);

UCLASS()
class ADVENTURE_API AWeapon : public AItem
{
	GENERATED_BODY()

public:
	AWeapon();
	void Equip(USceneComponent* Parent, const FName& SocketName, AActor* NewOwner, APawn* NewInstigator);
	void AttachMeshToSocket(USceneComponent* Parent, const FName& SocketName);
	bool TryFire();
	void ReloadAmmo();
	bool CanReload() const;
	bool CanFire() const;
	
	UPROPERTY(BlueprintAssignable)
	FOnAmmoChanged OnAmmoChanged;

protected:
	virtual void BeginPlay() override;
	FTimerHandle FireRateHandle;

	UPROPERTY(VisibleAnywhere, Category="Ammo")
	int32 CurrentAmmo = 15;

	UPROPERTY(EditDefaultsOnly, Category="Ammo")
	int32 MagazineCapacity = 15;

	UPROPERTY(EditDefaultsOnly, Category = "Ammo")
	int32 ReserveAmmo = 45;

private:
	void SpawnParticle() const;
	void ConsumeAmmo();
	void FireTrace(FHitResult& OutHit) const;
	void ResetFire();
	void StartFireCooldown();
	bool bCanFire = true;
	FVector GetShotDirection(const FVector& TargetPoint) const;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	UNiagaraSystem* FireEffect;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	float FireRate = 0.5f;

public:
	FORCEINLINE int GetCurrentAmmo() const { return CurrentAmmo; }
	FORCEINLINE int GetReserveAmmo() const { return ReserveAmmo; }
	FORCEINLINE int GetMagazineCapacity() const { return MagazineCapacity; }
};
