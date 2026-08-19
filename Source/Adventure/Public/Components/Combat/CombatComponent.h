#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Characters/CharacterTypes.h"
#include "CombatComponent.generated.h"

class ABaseCharacter;
class AWeapon;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ADVENTURE_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCombatComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void Fire();
	virtual void Reload();
	virtual void StartAiming();
	virtual void StopAiming();
	virtual void EquipWeapon(AWeapon* Weapon);
	virtual void SpawnDefaultWeapon();

	virtual bool CanReload() const;
	virtual bool CanFire() const;
	virtual bool CanAim() const;
	virtual bool CanArm() const;
	virtual bool CanDisarm() const;

	UFUNCTION(BlueprintCallable)
	virtual void Arm();
	
	UFUNCTION(BlueprintCallable)
	virtual void Disarm();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void FinishWeaponEquip();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void FinishWeaponUnequip();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void FinishReloading();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category="Combat")
	FName WeaponHandSocket = FName("RightHandSocket");

	UPROPERTY(EditDefaultsOnly, Category="Combat")
	FName WeaponHolsterSocket = FName("PistolSocket");

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TSubclassOf<class AWeapon> WeaponClass;

	UPROPERTY(EditDefaultsOnly, Category="Combat")
	bool bSpawnDefaultWeapon = false;

private:	
    UPROPERTY()
    ABaseCharacter* Character;

	UPROPERTY(VisibleAnywhere, Category = Weapon)
	AWeapon* EquippedWeapon;

	/*
	Combat State
	*/

	UPROPERTY(BlueprintReadOnly, Category="Combat", meta=(AllowPrivateAccess="true"))
	EWeaponState WeaponState = EWeaponState::EWS_Unarmed;

	UPROPERTY(BlueprintReadOnly, Category="Combat", meta=(AllowPrivateAccess="true"))
	ECombatState CombatState = ECombatState::ECS_Idle;
	
public:
	FORCEINLINE EWeaponState GetWeaponState() const { return WeaponState; }
	FORCEINLINE ECombatState GetCombatState() const { return CombatState; }
	FORCEINLINE void SetCombatState(ECombatState NewState){ CombatState = NewState;}
};
