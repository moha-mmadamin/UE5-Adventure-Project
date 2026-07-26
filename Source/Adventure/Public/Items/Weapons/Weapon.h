#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Weapon.generated.h"



class UBoxComponent;
class UNiagaraSystem;
class AEcho;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnAmmoChanged,
	int, CurrentAmmo,
	int, MaxAmmo
);


UCLASS()
class ADVENTURE_API AWeapon : public AItem
{
	GENERATED_BODY()

public:
	AWeapon();
	void Equip(USceneComponent* Parent, const FName& SocketName, AActor* NewOwner, APawn* NewInstigator);
	void AttachMeshToSocket(USceneComponent* Parent, const FName& SocketName);
	void Shoot();
	void ReloadAmmo();

	UPROPERTY(BlueprintAssignable)
	FOnAmmoChanged OnAmmoChanged;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	int CurrentAmmo = 15;

	UPROPERTY(VisibleAnywhere, Category="Ammo")
	int MagazineCapacity = 15;

	UPROPERTY(VisibleAnywhere, Category = "Ammo")
	int ReserveAmmo = 45;

	UPROPERTY(VisibleAnywhere)
	AEcho* Echo;

private:
	void StartReload();
	void SpawnParticle();
	void ConsumeAmmo();
	void FireTrace();
	bool CanShoot() const;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	UNiagaraSystem* FireEffect;

public:
	FORCEINLINE int GetCurrentAmmo() const { return CurrentAmmo; }
	FORCEINLINE void SetCurrentAmmo(int NewCurrentAmmo) { CurrentAmmo = NewCurrentAmmo; }
	FORCEINLINE int GetReserveAmmo() const { return ReserveAmmo; }
	FORCEINLINE void SetReserveAmmo(int NewReserveAmmo) { ReserveAmmo = NewReserveAmmo; }
	FORCEINLINE int GetMagazineCapacity() const { return MagazineCapacity; }
};
