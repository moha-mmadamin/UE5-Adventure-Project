#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "TimerManager.h"
#include "BaseWeapon.generated.h"

class UNiagaraSystem;
class UParticleSystem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnAmmoChanged,
	int32, CurrentAmmo,
	int32, ReserveAmmo
);

UCLASS()
class ADVENTURE_API ABaseWeapon : public AItem
{
	GENERATED_BODY()
	
public:
	ABaseWeapon();
	void Equip(USceneComponent* Parent, const FName& SocketName, AActor* NewOwner, APawn* NewInstigator);
	void AttachMeshToSocket(USceneComponent* Parent, const FName& SocketName);
	bool Fire();
	void ReloadAmmo();
	bool CanReloadAmmo() const;
	bool CanFire() const;

	UPROPERTY(BlueprintAssignable)
	FOnAmmoChanged OnAmmoChanged;

protected:
	virtual void BeginPlay();

	/*
	Weapon Behaviour
	*/

	//void FireTrace(FHitResult& OutHit) const;
	virtual float GetDamageForBone(FName BoneName) const;
	virtual void ConsumeAmmo();
	virtual void SpawnParticle() const;
	virtual void SpawnBlood(const FHitResult& HitResult);
	virtual void ApplyDamage(const FHitResult& HitResult);
	//FVector GetShotDirection(const FVector& TargetPoint) const;

	/*
	Ammo
	*/

	UPROPERTY(VisibleAnywhere, Category="Ammo")
	int32 CurrentAmmo = 15;

	UPROPERTY(EditDefaultsOnly, Category="Ammo")
	int32 MagazineCapacity = 15;

	UPROPERTY(EditDefaultsOnly, Category = "Ammo")
	int32 ReserveAmmo = 45;

	/*
	Fire
	*/

    UPROPERTY(EditDefaultsOnly, Category = "Weapon|Fire")
    float FireRate = 0.5f;

    UPROPERTY(EditDefaultsOnly, Category = "Weapon|Fire")
    float TraceDistance = 15000.f;

	/*
	Effect
	*/

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	UNiagaraSystem* FireEffect;

    UPROPERTY(EditAnywhere, Category = "Effects")
    UParticleSystem* BloodParticle;

	/*
	Cooldown
	*/

	FTimerHandle FireRateHandle;

	bool bCanFire = true;

	void StartFireCooldown();
	void ResetFire();

private:
	void GetCameraTrace(FHitResult& OutHit) const;

public:
	FORCEINLINE int GetCurrentAmmo() const { return CurrentAmmo; }
	FORCEINLINE int GetReserveAmmo() const { return ReserveAmmo; }
	FORCEINLINE int GetMagazineCapacity() const { return MagazineCapacity; }
};
