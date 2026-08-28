#include "Characters/BaseCharacter.h"
#include "Components/Combat/CombatComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	CombatComponent = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
}
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	

}
void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}
void ABaseCharacter::SetMovementSpeed(float Speed)
{
	if(GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = Speed;
	}
}
void ABaseCharacter::PlayFireMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
    if(AnimInstance && FireMontage)
    {
        AnimInstance->Montage_Play(FireMontage);
        AnimInstance->Montage_JumpToSection(SectionName, FireMontage);
    }
}
void ABaseCharacter::PlayEquipMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if(AnimInstance && EquipMontage)
	{
		AnimInstance->Montage_Play(EquipMontage);
		AnimInstance->Montage_JumpToSection(SectionName, EquipMontage);
	}
}
void ABaseCharacter::PlayReloadMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
    if(AnimInstance && ReloadMontage)
    {
        AnimInstance->Montage_Play(ReloadMontage);
        AnimInstance->Montage_JumpToSection(SectionName, ReloadMontage);
    }
}