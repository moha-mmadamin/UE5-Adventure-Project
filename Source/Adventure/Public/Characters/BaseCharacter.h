#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Animation/AnimInstance.h"
#include "BaseCharacter.generated.h"

class AItem;
class UCombatComponent;

UCLASS()
class ADVENTURE_API ABaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ABaseCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void PlayFireMontage(const FName& SectionName);
	void PlayEquipMontage(const FName& SectionName);
	void PlayReloadMontage(const FName& SectionName);

protected:
	virtual void BeginPlay() override;
	void SetMovementSpeed(float Speed);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category= "Combat", meta=(AllowPrivateAccess="true"))
	UCombatComponent* CombatComponent;

	/*
	Anim Montages
	*/

	UPROPERTY(EditDefaultsOnly, Category = Montages)
    UAnimMontage* ReloadMontage;

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* EquipMontage;
	
	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* FireMontage;

	UPROPERTY(VisibleInstanceOnly)
	AItem* OverlappingItem;

public:	
	FORCEINLINE void SetOverlappingItem(AItem* Item) { OverlappingItem = Item; }
	FORCEINLINE UCombatComponent* GetCombatComponent() const { return CombatComponent; }
	
};
