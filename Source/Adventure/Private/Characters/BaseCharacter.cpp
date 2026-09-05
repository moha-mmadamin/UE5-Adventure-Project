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
int32 ABaseCharacter::PlayDeathMontage()
{
	const int32 Selection = PlayRandomMontageSection(DeathMontage, DeathMontageSections);
	TEnumAsByte<EDeathPose> Pose(Selection);
	if(Pose < EDeathPose::EDP_Max)
	{
		DeathPose = Pose;
	}
	return Selection;
}
void ABaseCharacter::Die()
{
	Tags.Add(FName("Dead"));
	CharacterState = ECharacterState::ECS_Dead;
	PlayDeathMontage();
}
void ABaseCharacter::PlayMontageSection(UAnimMontage* Montage, const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if(Montage)
	{
		AnimInstance->Montage_Play(Montage);
		AnimInstance->Montage_JumpToSection(SectionName, Montage);
	}
}
int32 ABaseCharacter::PlayRandomMontageSection(UAnimMontage* Montage, const TArray<FName>& SectionNames)
{
	if(SectionNames.Num() <= 0) return - 1;
	const int32 MaxSectionIndex = SectionNames.Num() - 1;
	const int32 Selection = FMath::RandRange(0, MaxSectionIndex);
	PlayMontageSection(Montage, SectionNames[Selection]);
	return Selection;
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