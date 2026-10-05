#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Characters/CharacterTypes.h"
#include "CombatComponent.generated.h"

// Forward Declarations
class ABaseCharacter;
class ABaseWeapon;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ADVENTURE_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCombatComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/*
	Combat Actions
	*/

	UFUNCTION(BlueprintCallable)
	void Fire();

	void Reload();

	UFUNCTION(BlueprintCallable)
	void Disarm();

	UFUNCTION(BlueprintCallable)
	void Arm();

	UFUNCTION(BlueprintCallable)
	void StartAiming();

	UFUNCTION(BlueprintCallable)
	void StopAiming();

	void EquipWeapon(ABaseWeapon* Weapon);
	void SpawnDefaultWeapon();
	void ToggleWeapon();

	/*
	State Checks
	*/

	bool CanReload() const;
	bool CanFire() const;
	bool CanAim() const;
	bool CanArm() const;
	bool CanDisarm() const;

	/*
	Anim Notify
	*/

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void FinishWeaponEquip();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void FinishWeaponUnequip();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void FinishReloading();

protected:
	virtual void BeginPlay() override;

	/*
	Sockets
	*/

	UPROPERTY(EditDefaultsOnly, Category="Combat")
	FName WeaponHandSocket = FName("RightHandSocket");

	UPROPERTY(EditDefaultsOnly, Category="Combat")
	FName WeaponHolsterSocket = FName("PistolSocket");

	/*
	Weapon Setup
	*/

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TSubclassOf<class ABaseWeapon> WeaponClass;

	UPROPERTY(EditDefaultsOnly, Category="Combat")
	bool bSpawnDefaultWeapon = false;

private:	
    UPROPERTY()
    ABaseCharacter* Character;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon, meta=(AllowPrivateAccess="true"))
	ABaseWeapon* EquippedWeapon;

	/*
	State
	*/

	UPROPERTY(BlueprintReadWrite, Category="Combat", meta=(AllowPrivateAccess="true"))
	EWeaponState WeaponState = EWeaponState::EWS_Unarmed;

	UPROPERTY(BlueprintReadWrite, Category="Combat", meta=(AllowPrivateAccess="true"))
	ECombatState CombatState = ECombatState::ECS_Idle;
	
public:
	FORCEINLINE EWeaponState GetWeaponState() const { return WeaponState; }
	FORCEINLINE ECombatState GetCombatState() const { return CombatState; }
	FORCEINLINE ABaseWeapon* GetEquippedWeapon() const { return EquippedWeapon; }
	FORCEINLINE void SetCombatState(ECombatState NewState){ CombatState = NewState; }
};
