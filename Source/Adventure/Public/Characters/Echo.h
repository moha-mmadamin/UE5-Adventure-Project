#pragma once

#include "CharacterTypes.h"
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EchoAnimInstance.h"
#include "InputActionValue.h"
#include "UI/AmmoWidget.h"
#include "UI/HealthBarWidget.h"
#include "Components/HealthComponent.h"
#include "Echo.generated.h"


class USpringArmComponent;
class UCameraComponent;
class UGroomComponent;
class AWeapon;
class UAnimMontage;
class AItem;
class UInputMappingContext;
class UInputAction;
class AActor;

UCLASS()
class ADVENTURE_API AEcho : public ACharacter
{
	GENERATED_BODY()
public:
	AEcho();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    bool bHasWeapon = true;

	
	void PlayReloadMontage(const FName& SectionName);

 protected:
	virtual void BeginPlay() override;
	void EquipWeapon(AWeapon* Weapon);
	void PlayEquipMontage(const FName& SectionName);
	void PlayFireMontage(const FName& SectionName);
	bool CanArm();
	bool CanDisarm();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsAiming = false;

	UFUNCTION(BlueprintCallable)
	void FinishEquipping();

	UFUNCTION(BlueprintCallable)
	void FinishReloading();
	
	UFUNCTION(BlueprintCallable)
	void Arm();
	
	UFUNCTION(BlueprintCallable)
	void Disarm();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USpringArmComponent* CameraBoom;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UCameraComponent* ViewCamera;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputMappingContext* EchoContex;

	// Input actions
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	virtual void Jump() override;
	void EKeyPressed();
	void Fire();
	void Aim();
	void StopAiming();
	void Sprint();
	void StopSprint();
    void Reload();

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* MovementAction;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* FireAction;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* EkeyAction;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* AimAction;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* SprintAction;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* ReloadAction;

private:
	void SpawnDefaultWeapon();

	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta = (AllowPrivateAccess = "true"))
	ECharacterState CharacterState = ECharacterState::ECS_EquippedGun;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	EActionState ActionState = EActionState::EAS_Unoccupied;
	
	UPROPERTY(VisibleAnywhere, Category = Hair)
	UGroomComponent* Hair;
	
	UPROPERTY(VisibleAnywhere, Category = Hair)
	UGroomComponent* Eyebrows;
	
	UPROPERTY(VisibleInstanceOnly)
	AItem* OverlappingItem;

	UPROPERTY(VisibleAnywhere, Category = Weapon)
	AWeapon* EquippedWeapon;
	
	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* EquipMontage;
	
	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* FireMontage;

	UPROPERTY(EditDefaultsOnly, Category = Montages)
    UAnimMontage* ReloadMontage;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AWeapon> WeaponClass;

    UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UAmmoWidget> AmmoWidgetClass;

	UAmmoWidget* AmmoWidget;

	UPROPERTY(VisibleAnywhere, Category = "Health")
    UHealthComponent* HealthComponent;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UHealthBarWidget> HealthWidgetClass;

    UHealthBarWidget* HealthWidget;

public:
	FORCEINLINE void SetOverlappingItem(AItem* Item) { OverlappingItem = Item; }
	FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }
	FORCEINLINE EActionState GetActionState() const { return ActionState; }
	FORCEINLINE void SetActionState(EActionState NewState){ ActionState = NewState; }
};