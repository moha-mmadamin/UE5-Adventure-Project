#pragma once

#include "CharacterTypes.h"
#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "InputActionValue.h"
#include "UI/AmmoWidget.h"
#include "UI/HealthBarWidget.h"
#include "Components/Health/HealthComponent.h"
#include "Echo.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UGroomComponent;
class AWeapon;
class UAnimMontage;
class UInputMappingContext;
class UInputAction;
class AActor;
class UAIPerceptionStimuliSourceComponent;

UCLASS()
class ADVENTURE_API AEcho : public ABaseCharacter
{
	GENERATED_BODY()
public:
	AEcho();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void DeathFinished();

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
	void StartAiming();
	void StopAiming();
	void Sprint();
	void StopSprint();
	void Fire();
	void Reload();
	void ShowCrosshairWidget();
	void HideCrosshairWidget();
	void ShowAmmoWidget();
	void HideAmmoWidget();

	UFUNCTION(BlueprintCallable)
	virtual void Die() override;

	UPROPERTY(EditAnywhere, Category = "Sensitivity")
	float LookSensitivity = 1.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	UAIPerceptionStimuliSourceComponent* StimuliSourceComponent;
	
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

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> DeathScreenClass;

	UUserWidget* DeathScreenWidget;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> CrosshairClass;

	UUserWidget* CrosshairWidget;
};