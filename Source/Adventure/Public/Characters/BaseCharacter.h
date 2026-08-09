#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
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
	virtual bool CanArm();
	virtual bool CanDisarm();
	virtual void Fire();
	virtual void Aim();
	virtual void StopAiming();
	virtual void Reload();
	virtual void EquipWeapon(AWeapon* Weapon);
	virtual void PlayFireMontage(const FName& SectionName);
	void PlayEquipMontage(const FName& SectionName);
	void PlayReloadMontage(const FName& SectionName);
	void SetMovementSpeed(float Speed);
	virtual void SpawnDefaultWeapon();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    bool bHasWeapon = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsAiming = false;

	UFUNCTION(BlueprintCallable)
	virtual void Arm();
	
	UFUNCTION(BlueprintCallable)
	virtual void Disarm();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void FinishEquipping();

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
	Weapon class
	*/

	UPROPERTY(VisibleAnywhere, Category = Weapon)
	AWeapon* EquippedWeapon;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AWeapon> WeaponClass;

	UPROPERTY(VisibleInstanceOnly)
	AItem* OverlappingItem;

public:	
	FORCEINLINE void SetOverlappingItem(AItem* Item) {OverlappingItem = Item;}

};
