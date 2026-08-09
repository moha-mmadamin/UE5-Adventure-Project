#pragma once

#include "CharacterTypes.h"
#include "CoreMinimal.h"
#include "BaseCharacter.h"
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
class UInputMappingContext;
class UInputAction;
class AActor;

UCLASS()
class ADVENTURE_API AEcho : public ABaseCharacter
{
	GENERATED_BODY()
public:
	AEcho();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

 protected:
	virtual void BeginPlay() override;

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

	/*
	Input Actions
	*/

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
	virtual void Fire() override;
	virtual void Aim() override;
	virtual void StopAiming() override;
	virtual void Reload() override;
	virtual void EquipWeapon(AWeapon* Weapon) override;
	virtual void SpawnDefaultWeapon() override;
	virtual bool CanArm() override;
	virtual bool CanDisarm() override;
	virtual void FinishReloading_Implementation() override;
	virtual void FinishEquipping_Implementation() override;
	void Sprint();
	void StopSprint();
	bool CanFire() const;
	bool CanReload() const;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta = (AllowPrivateAccess = "true"))
	ECharacterState CharacterState = ECharacterState::ECS_EquippedGun;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	EActionState ActionState = EActionState::EAS_Unoccupied;
	
	UPROPERTY(VisibleAnywhere, Category = Hair)
	UGroomComponent* Hair;
	
	UPROPERTY(VisibleAnywhere, Category = Hair)
	UGroomComponent* Eyebrows;

    UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UAmmoWidget> AmmoWidgetClass;

	UAmmoWidget* AmmoWidget;

	UPROPERTY(VisibleAnywhere, Category = "Health")
    UHealthComponent* HealthComponent;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UHealthBarWidget> HealthWidgetClass;

    UHealthBarWidget* HealthWidget;

public:
	FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }
	FORCEINLINE EActionState GetActionState() const { return ActionState; }
	FORCEINLINE void SetActionState(EActionState NewState){ ActionState = NewState; }
};