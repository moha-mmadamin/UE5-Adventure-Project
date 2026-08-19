#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Characters/CharacterTypes.h"
#include "BaseCharacter.generated.h"

class AWeapon;
class AItem;

UCLASS()
class ADVENTURE_API ABaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ABaseCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
protected:
	virtual void BeginPlay() override;
	virtual void Fire();
	virtual void StartAiming();
	virtual void StopAiming();
	virtual void Reload();
	virtual void EquipWeapon(AWeapon* Weapon);
	virtual void PlayFireMontage(const FName& SectionName);
	virtual bool CanArm();
	virtual bool CanDisarm();
	virtual bool CanReload() const;
	virtual bool CanFire() const;
	virtual bool CanAim() const;
	void PlayEquipMontage(const FName& SectionName);
	void PlayReloadMontage(const FName& SectionName);
	void SetMovementSpeed(float Speed);
	virtual void SpawnDefaultWeapon();

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

	/*
	Anim Montages
	*/

	UPROPERTY(EditDefaultsOnly, Category = Montages)
    UAnimMontage* ReloadMontage;

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* EquipMontage;
	
	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* FireMontage;

	/*
	 Combat State
	*/

	UPROPERTY(BlueprintReadOnly, Category="Combat")
	EWeaponState WeaponState = EWeaponState::EWS_Unarmed;


	UPROPERTY(BlueprintReadOnly, Category="Combat")
	ECombatState CombatState = ECombatState::ECS_Idle;

	/*
	Weapon class
	*/

	UPROPERTY(VisibleAnywhere, Category = Weapon)
	AWeapon* EquippedWeapon;
	
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TSubclassOf<class AWeapon> WeaponClass;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	bool bSpawnDefaultWeapon = false;

	UPROPERTY(EditDefaultsOnly, Category="Combat")
	FName WeaponHandSocket = FName("RightHandSocket");

	UPROPERTY(EditDefaultsOnly, Category="Combat")
	FName WeaponHolsterSocket = FName("PistolSocket");

	UPROPERTY(VisibleInstanceOnly)
	AItem* OverlappingItem;

public:	
	FORCEINLINE void SetOverlappingItem(AItem* Item) {OverlappingItem = Item;}
	FORCEINLINE EWeaponState GetWeaponState() const { return WeaponState; }
	FORCEINLINE ECombatState GetCombatState() const { return CombatState; }
	FORCEINLINE void SetCombatState(ECombatState NewState){ CombatState = NewState; }

};
