#include "Characters/Enemy.h"
#include "GameFramework/CharacterMovementComponent.h"

AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

}
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

}
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
void AEnemy::UpdateEnemyState()
{
    switch (EnemyDetectionType)
    {
        case EEnemyDetectionType::EDT_Sight:
            EnemyState = EEnemyState::EES_Chase;
            break;

        case EEnemyDetectionType::EDT_Hearing:
            EnemyState = EEnemyState::EES_Investigate;
            break;

        case EEnemyDetectionType::EDT_Damage:
            EnemyState = EEnemyState::EES_Chase;
            break;

        default:
            break;
    }
}
void AEnemy::HandleSight(AActor* DetectedActor)
{
    if(!DetectedActor) return;
    EnemyDetectionType = EEnemyDetectionType::EDT_Sight;

    CurrentTarget = DetectedActor;

    LastKnownLocation = DetectedActor->GetActorLocation();

    UpdateEnemyState();
}
void AEnemy::HandleHearing(const FVector& Location)
{
    EnemyDetectionType = EEnemyDetectionType::EDT_Hearing;

    LastKnownLocation = Location;

    UpdateEnemyState();
}
void AEnemy::HandleDamage(AActor* DamageCauser)
{
    EnemyDetectionType = EEnemyDetectionType::EDT_Damage;

    CurrentTarget = DamageCauser;

    UpdateEnemyState();
}

