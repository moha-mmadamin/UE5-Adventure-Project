#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Animation/AnimInstance.h"
#include "CharacterTypes.h"
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

	void SetMovementSpeed(float Speed);

	void PlayFireMontage(const FName& SectionName);
	void PlayEquipMontage(const FName& SectionName);
	void PlayReloadMontage(const FName& SectionName);

protected:
	virtual void BeginPlay() override;
	virtual int32 PlayDeathMontage();
	virtual void Die();

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

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* DeathMontage;

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	TArray<FName> DeathMontageSections;

	UPROPERTY(BlueprintReadOnly)
	TEnumAsByte<EDeathPose> DeathPose;

	UPROPERTY(VisibleInstanceOnly)
	AItem* OverlappingItem;

	/*
	State
	*/
	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	ECharacterState CharacterState = ECharacterState::ECS_Alive;

private:
	void PlayMontageSection(UAnimMontage* Montage, const FName& SectionName);
	int32 PlayRandomMontageSection(UAnimMontage* Montage, const TArray<FName>& SectionNames);

public:	
	FORCEINLINE void SetOverlappingItem(AItem* Item) { OverlappingItem = Item; }
	FORCEINLINE UCombatComponent* GetCombatComponent() const { return CombatComponent; }
	FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }
	
};
