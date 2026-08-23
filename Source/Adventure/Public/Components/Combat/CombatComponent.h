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

	void Fire();
	void Reload();
	void StartAiming();
	void StopAiming();
	void EquipWeapon(AWeapon* Weapon);
	void SpawnDefaultWeapon();
	void ToggleWeapon();

	bool CanReload() const;
	bool CanFire() const;
	bool CanAim() const;
	bool CanArm() const;
	bool CanDisarm() const;

	UFUNCTION(BlueprintCallable)
	void Arm();
	
	UFUNCTION(BlueprintCallable)
	void Disarm();

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
	FORCEINLINE AWeapon* GetEquippedWeapon() const { return EquippedWeapon; }
	FORCEINLINE void SetCombatState(ECombatState NewState){ CombatState = NewState; }
};
