#include "AI/EnemyAIController.h"

#include "Characters/Enemy.h"
#include "Characters/EnemyTypes.h"

#include "Components/Combat/CombatComponent.h"

#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISense_Hearing.h"
#include "Perception/AISense_Damage.h"

#include "BehaviorTree/BlackboardComponent.h"

AEnemyAIController::AEnemyAIController()
{
}
void AEnemyAIController::BeginPlay()
{
    Super::BeginPlay();

    AIPerceptionComponent = GetPerceptionComponent();

    if (!AIPerceptionComponent)
    {
        if (APawn* ControlledPawn = GetPawn())
        {
            AIPerceptionComponent = ControlledPawn->FindComponentByClass<UAIPerceptionComponent>();
        }
    }
    if (!AIPerceptionComponent) return;

    AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(
        this,
        &AEnemyAIController::OnTargetPerceptionUpdated
    );
    SetEnemyState(EEnemyState::EES_Patrol);
}
void AEnemyAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
    if(!Actor) return;

    if (Stimulus.Type == UAISense::GetSenseID<UAISense_Sight>())
    {
        HandleSight(Actor, Stimulus);
        return;
    }

    if (Stimulus.Type == UAISense::GetSenseID<UAISense_Hearing>())
    {
        HandleHearing(Stimulus);
        return;
    }

    if (Stimulus.Type == UAISense::GetSenseID<UAISense_Damage>())
    {
        HandleDamage(Actor, Stimulus);
        return;
    }
}

void AEnemyAIController::HandleSight(AActor* DetectedActor, const FAIStimulus& Stimulus)
{
    if(!DetectedActor) return;

    if(Stimulus.WasSuccessfullySensed())
    {
        bHasSightTarget = true;
        SetTargetActor(DetectedActor);
        SetEnemyState(EEnemyState::EES_Combat);
    }
    else
    {
        bHasSightTarget = false;
        SetLastKnownLocation(Stimulus.StimulusLocation);
        ClearTargetActor();
        SetEnemyState(EEnemyState::EES_Investigating);
    }
}
void AEnemyAIController::HandleHearing(const FAIStimulus& Stimulus)
{
    if(bHasSightTarget) return;

    SetLastKnownLocation(Stimulus.StimulusLocation);
    ClearTargetActor();
    SetEnemyState(EEnemyState::EES_Investigating);
}
void AEnemyAIController::HandleDamage(AActor* DamageCauser, const FAIStimulus& Stimulus)
{
    if(!DamageCauser) return;

    SetTargetActor(DamageCauser);
    SetEnemyState(EEnemyState::EES_Combat);
}
void AEnemyAIController::SetTargetActor(AActor* NewTarget)
{
    if(!NewTarget) return;

    UBlackboardComponent* BB = GetBlackboardComponent();
    if(!BB) return;

    BB->SetValueAsObject(TEXT("TargetActor"), NewTarget);
    BB->SetValueAsVector(TEXT("LastKnownLocation"), NewTarget->GetActorLocation());
}
void AEnemyAIController::SetLastKnownLocation(const FVector& Location)
{
    UBlackboardComponent* BB = GetBlackboardComponent();
    if(!BB) return;

    BB->SetValueAsVector(TEXT("LastKnownLocation"), Location);
}
void AEnemyAIController::ClearTargetActor()
{
    UBlackboardComponent* BB = GetBlackboardComponent();
    if(!BB) return;

    BB->ClearValue(TEXT("TargetActor"));
}
void AEnemyAIController::OnStateChanged(EEnemyState PreviousState, EEnemyState NewState)
{
    AEnemy* Enemy = Cast<AEnemy>(GetPawn());
    if(!Enemy) return;

    switch (NewState)
    {
    case EEnemyState::EES_Searching:
    case EEnemyState::EES_Combat:
    case EEnemyState::EES_Patrol:
    case EEnemyState::EES_Investigating:

        Enemy->SetMovementSpeed(PatrolSpeed);
        break;

    case EEnemyState::EES_TakingCover:
    case EEnemyState::EES_Chasing:

        Enemy->SetMovementSpeed(ChaseSpeed);
        break;

    case EEnemyState::EES_Idle:

        Enemy->SetMovementSpeed(0.f);
        break;

    default:
        break;
    }
}
AActor* AEnemyAIController::SelectNextPatrolTarget()
{
    AEnemy* Enemy = Cast<AEnemy>(GetPawn());
    if(!Enemy) return nullptr;

    const TArray<AActor*>& Targets = Enemy->GetPatrolTargets();

    if(Targets.Num() == 0) return nullptr;

    TArray<AActor*> ValidTargets;

    for (AActor* Target : Targets)
    {
        if (Target && Target != PatrolTarget)
        {
            ValidTargets.AddUnique(Target);
        }
    }

    const int32 NumPatrolTargets = ValidTargets.Num();

    if (NumPatrolTargets > 0)
    {
        const int32 TargetSelection = FMath::RandRange(0, NumPatrolTargets - 1);
        return ValidTargets[TargetSelection];
    }
    return Targets[0];
}
void AEnemyAIController::SetNextPatrolTargetOnBlackboard()
{
    SetEnemyState(EEnemyState::EES_Patrol);

    UBlackboardComponent* BB = GetBlackboardComponent();
    if (!BB) return;

    AActor* NextTarget = SelectNextPatrolTarget();
    if (NextTarget)
    {
        PatrolTarget = NextTarget;
        BB->SetValueAsObject(TEXT("PatrolTarget"), NextTarget);
    }
}
void AEnemyAIController::LookAround()
{
    AEnemy* Enemy = Cast<AEnemy>(GetPawn());
    if(!Enemy) return;

    LookAroundStep = 0;
    LookAroundStartYaw = Enemy->GetActorRotation().Yaw;

    GetWorldTimerManager().SetTimer(
        LookAroundTimer,
        this,
        &AEnemyAIController::FinishLookAround,
        LookAroundDelay,
        false
    );
}
bool AEnemyAIController::IsLookingAround() const
{
    return GetWorldTimerManager().IsTimerActive(LookAroundTimer);
}
void AEnemyAIController::FinishLookAround()
{
    AEnemy* Enemy = Cast<AEnemy>(GetPawn());
    if (!Enemy)
    {
        GetWorldTimerManager().ClearTimer(LookAroundTimer);
        return;
    }

    switch (LookAroundStep)
    {
    case 0:
    {
        const float LeftYaw = LookAroundStartYaw - LookAroundAngle;
        SetControlRotation(FRotator(0.f, LeftYaw, 0.f));
        break;
    }
    case 1:
    {
        const float RightYaw = LookAroundStartYaw + LookAroundAngle;
        SetControlRotation(FRotator(0.f, RightYaw, 0.f));
        break;
    }
    case 2:
        SetControlRotation(FRotator(0.f, LookAroundStartYaw, 0.f));
        break;

    case 3:
        GetWorldTimerManager().ClearTimer(LookAroundTimer);
        SetEnemyState(EEnemyState::EES_Patrol);
        if (Enemy->GetCombatComponent())
        {
            Enemy->GetCombatComponent()->StopAiming();
            Enemy->GetCombatComponent()->Disarm();
        }
        return;
    }

    LookAroundStep++;

    GetWorldTimerManager().SetTimer(
        LookAroundTimer,
        this,
        &AEnemyAIController::FinishLookAround,
        LookAroundDelay,
        false
    );
}
void AEnemyAIController::SetEnemyState(EEnemyState NewState)
{
    const EEnemyState PreviousState = EnemyState;
    EnemyState = NewState;

    if (UBlackboardComponent* BB = GetBlackboardComponent())
    {
        BB->SetValueAsEnum(TEXT("EnemyState"), static_cast<uint8>(NewState));
    }

    if (PreviousState != NewState)
    {
        OnStateChanged(PreviousState, NewState);
    }
}