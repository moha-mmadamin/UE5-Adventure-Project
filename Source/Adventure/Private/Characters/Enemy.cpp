#include "Characters/Enemy.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"
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

    EnemyController = Cast<AAIController>(GetController());
    MoveToTarget(PatrolTarget);
}
void AEnemy::MoveToTarget(AActor* Target)
{
    if(EnemyController == nullptr || Target == nullptr) return;
    FAIMoveRequest MoveRequest;
    MoveRequest.SetGoalActor(Target);
    MoveRequest.SetAcceptanceRadius(15.f);
    EnemyController->MoveTo(MoveRequest);
}
AActor* AEnemy::ChoosePatrolTarget()
{
    TArray<AActor*> ValidTargets;
    for (AActor* Target : PatrolTargets)
    {
        if(Target != PatrolTarget)
        {
            ValidTargets.AddUnique(Target);
        }
    }

    const int32 NumPatrolTargets = ValidTargets.Num();
    if(NumPatrolTargets > 0)
    {
        const int32 TargetSelection = FMath::RandRange(0, NumPatrolTargets - 1);
        return ValidTargets[TargetSelection];
    }
    return nullptr;
}
bool AEnemy::InTargetRange(AActor* Target, double Radius)
{
    if (Target == nullptr) return false;
    const double DistanceToTarget = (Target->GetActorLocation() - GetActorLocation()).Size();
    return DistanceToTarget <= Radius;
}
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
    
    CheckPatrolTarget();
}
void AEnemy::CheckPatrolTarget()
{
    if(InTargetRange(PatrolTarget, PatrolRadius))
    {
        PatrolTarget = ChoosePatrolTarget();
        const float WaitTime = FMath::RandRange(WaitMin, WaitMax);
        GetWorldTimerManager().SetTimer(PatrolTimer, this, &AEnemy::PatrolTimerFinished, WaitTime);
    }
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
void AEnemy::PatrolTimerFinished()
{
    MoveToTarget(PatrolTarget);
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

