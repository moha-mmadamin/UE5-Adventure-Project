#include "Characters/Enemy.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISense_Hearing.h"
#include "Perception/AISense_Damage.h"
#include "GameFramework/CharacterMovementComponent.h"

AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
    
    AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComponent"));
}
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

    if(AIPerceptionComponent)
    {
        AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(
            this,
            &AEnemy::OnTargetPerceptionUpdated
        );
    }
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
void AEnemy::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
    if(!Actor) return;

    if(Stimulus.Type == UAISense::GetSenseID<UAISense_Sight>())
    {
        HandleSight(Actor);
    }
    else if(Stimulus.Type == UAISense::GetSenseID<UAISense_Hearing>())
    {
        HandleHearing(Stimulus.StimulusLocation);
    }
    else if(Stimulus.Type == UAISense::GetSenseID<UAISense_Damage>())
    {
        HandleDamage(Actor);
    }
}
void AEnemy::HandleSight(AActor* DetectedActor)
{
    if(!DetectedActor) return;
    EnemyDetectionType = EEnemyDetectionType::EDT_Sight;

    CurrentTarget = DetectedActor;

    LastKnownLocation = DetectedActor->GetActorLocation();

    UpdateEnemyState();
    UE_LOG(LogTemp, Warning, TEXT("Enemy detected %s by sight."), *DetectedActor->GetName());
}
void AEnemy::HandleHearing(const FVector& Location)
{
    EnemyDetectionType = EEnemyDetectionType::EDT_Hearing;

    LastKnownLocation = Location;

    UpdateEnemyState();
}
void AEnemy::HandleDamage(AActor* DamageCauser)
{
    if(!DamageCauser) return;

    EnemyDetectionType = EEnemyDetectionType::EDT_Damage;

    CurrentTarget = DamageCauser;

    UpdateEnemyState();
}

