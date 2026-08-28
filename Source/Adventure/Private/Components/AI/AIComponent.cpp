#include "Components/AI/AIComponent.h"
#include "Characters/Enemy.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "Components/Combat/CombatComponent.h"
#include "Components/Perception/PerceptionComponent.h"
#include "DrawDebugHelpers.h"
#include "Items/Weapons/Weapon.h"

UAIComponent::UAIComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}
void UAIComponent::BeginPlay()
{
	Super::BeginPlay();

    OwnerEnemy = Cast<AEnemy>(GetOwner());

    if(!OwnerEnemy) return;

    EnemyController = Cast<AAIController>(OwnerEnemy->GetController());
    CombatComponent = OwnerEnemy->GetCombatComponent();
    PerceptionComponent = OwnerEnemy->GetPerceptionComponent();

    if (PerceptionComponent)
    {
        PerceptionComponent->OnSightDetected.AddUObject(this, &UAIComponent::HandleSight);
        PerceptionComponent->OnSightLost.AddUObject(this, &UAIComponent::HandleLostSight);
        PerceptionComponent->OnNoiseHeard.AddUObject(this, &UAIComponent::HandleHearing);
        PerceptionComponent->OnDamaged.AddUObject(this, &UAIComponent::HandleDamage);
    }

    if (EnemyController)
    {
        EnemyController->GetPathFollowingComponent()->OnRequestFinished.AddUObject(
            this,
            &UAIComponent::OnMoveCompleted
        );
    }
}
void UAIComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}
void UAIComponent::HandleSight(AActor* DetectedActor)
{
    if(!DetectedActor) return;

    EnemyDetectionType = EEnemyDetectionType::EDT_Sight;

    CurrentTarget = DetectedActor;

    LastKnownLocation = DetectedActor->GetActorLocation();

    if (EnemyController)
    {
        EnemyController->SetFocus(CurrentTarget);
    }
    StartChasing();
}
void UAIComponent::HandleLostSight(AActor* DetectedActor)
{
    if(!DetectedActor) return;
    if(DetectedActor != CurrentTarget) return;

    LastKnownLocation = DetectedActor->GetActorLocation();
    CurrentTarget = nullptr;
    EndCombat();

    if (EnemyController)
    {
        EnemyController->ClearFocus(EAIFocusPriority::Gameplay);
    }

    BeginSearch();
}
void UAIComponent::HandleHearing(const FVector& Location)
{
    EnemyDetectionType = EEnemyDetectionType::EDT_Hearing;
    LastKnownLocation = Location;
    SetEnemyState(EEnemyState::EES_Investigating);

    if (EnemyController)
    {
        EnemyController->ClearFocus(EAIFocusPriority::Gameplay);
        EnemyController->MoveToLocation(LastKnownLocation,50.f);
    }
}
void UAIComponent::HandleDamage(AActor* DamageCauser)
{
    if(!DamageCauser) return;

    EnemyDetectionType = EEnemyDetectionType::EDT_Damage;
    CurrentTarget = DamageCauser;
    LastKnownLocation = DamageCauser->GetActorLocation();
    GetWorld()->GetTimerManager().ClearTimer(PatrolTimer);

    if (EnemyController)
    {
        EnemyController->SetFocus(CurrentTarget);
    }

    if (IsTargetInAttackRange())
    {
        EnterCombat();
    }
    else
    {
        StartChasing();
    }
}
void UAIComponent::SetEnemyState(EEnemyState NewState)
{
    if (EnemyState == NewState) return;

    const EEnemyState PreviousState = EnemyState;

    EnemyState = NewState;

    OnStateChanged(PreviousState,NewState);
}
void UAIComponent::OnStateChanged(EEnemyState PreviousState, EEnemyState NewState)
{
    switch (NewState)
    {
        case EEnemyState::EES_Searching:
        case EEnemyState::EES_Investigating:
        case EEnemyState::EES_Combat:
        case EEnemyState::EES_Patrol:

            OwnerEnemy->SetMovementSpeed(PatrolSpeed);
            break;

        case EEnemyState::EES_TakingCover:
        case EEnemyState::EES_Chasing:

            OwnerEnemy->SetMovementSpeed(ChaseSpeed);
            break;

        case EEnemyState::EES_Idle:
        case EEnemyState::EES_Dead:

            OwnerEnemy->SetMovementSpeed(0.f);
            break;

        default:
            break;
    }
}
void UAIComponent::StartChasing()
{
    if(!CurrentTarget) return;

    GetWorld()->GetTimerManager().ClearTimer(PatrolTimer);
    SetEnemyState(EEnemyState::EES_Chasing);
    MoveToActor(CurrentTarget);

    if (!GetWorld()->GetTimerManager().IsTimerActive(ChaseTimer))
    {
        GetWorld()->GetTimerManager().SetTimer(ChaseTimer,this,&UAIComponent::CheckChaseDistance,0.2f,true);
    }
}
void UAIComponent::EnterCombat()
{
    if(!CurrentTarget) return;

    GetWorld()->GetTimerManager().ClearTimer(ChaseTimer);
    SetEnemyState(EEnemyState::EES_Combat);

    if (EnemyController)
    {
        EnemyController->StopMovement();
        EnemyController->SetFocus(CurrentTarget);
    }

    CheckLineOfSight();

    if (!GetWorld()->GetTimerManager().IsTimerActive(LOSTimer))
    {
        GetWorld()->GetTimerManager().SetTimer(LOSTimer,this,&UAIComponent::CheckLineOfSight,0.1f,true);
    }
}
void UAIComponent::BeginCombat()
{
    if(!CombatComponent) return;

    //if(CombatComponent->GetEquippedWeapon()->GetCurrentAmmo() <= 0)
    //{
    //    CombatComponent->StopAiming();
    //    CombatComponent->Reload();
    //    return;
    //}
    //if(CombatComponent->GetCombatState() == ECombatState::ECS_Reloading) return;

    //CombatComponent->Arm();
    //CombatComponent->StartAiming();
    //CombatComponent->Fire();
}
void UAIComponent::EndCombat()
{
    GetWorld()->GetTimerManager().ClearTimer(ChaseTimer);
    GetWorld()->GetTimerManager().ClearTimer(LOSTimer);

    if(!CombatComponent) return;

    CombatComponent->StopAiming();
}
void UAIComponent::MoveToActor(AActor* Target)
{
    if(!EnemyController || !Target) return;

    FAIMoveRequest MoveRequest;
    MoveRequest.SetGoalActor(Target);
    MoveRequest.SetAcceptanceRadius(ChaseAcceptanceRadius);
    EnemyController->MoveTo(MoveRequest);
}
AActor* UAIComponent::SelectNextPatrolTarget()
{
    TArray<AActor*> ValidTargets;

    for (AActor* Target : PatrolTargets)
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
    return nullptr;
}
void UAIComponent::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
    if(!Result.IsSuccess()) return;

    if (EnemyState == EEnemyState::EES_Patrol)
    {
        PatrolTarget = SelectNextPatrolTarget();
        if(!PatrolTarget) return;
        const float WaitTime = FMath::RandRange(WaitMin,WaitMax);
        GetWorld()->GetTimerManager().SetTimer(PatrolTimer,this,&UAIComponent::OnPatrolWaitFinished,WaitTime,false);
    }
    else if (EnemyState == EEnemyState::EES_Chasing)
    {
        if (IsTargetInAttackRange())
        {
            EnterCombat();
        }
        else
        {
            StartChasing();
        }
    }
}
void UAIComponent::OnPatrolWaitFinished()
{
    if(!PatrolTarget) return;
    MoveToActor(PatrolTarget);
}
void UAIComponent::InitializeAI()
{
    if (!OwnerEnemy)
    {
        OwnerEnemy = Cast<AEnemy>(GetOwner());
    }
    if(!OwnerEnemy) return;

    EnemyController = Cast<AAIController>(OwnerEnemy->GetController());
    if(!EnemyController) return;

    EnemyController->GetPathFollowingComponent()->OnRequestFinished.AddUObject(
            this,
            &UAIComponent::OnMoveCompleted
        );

    if(PatrolTarget)
    {
        SetEnemyState(EEnemyState::EES_Patrol);
        MoveToActor(PatrolTarget);
    }
    else
    {
        SetEnemyState(EEnemyState::EES_Idle);
    }
}
void UAIComponent::BeginSearch()
{
    if(!EnemyController) return;

    SetEnemyState(EEnemyState::EES_Searching);
    EnemyController->MoveToLocation(LastKnownLocation,50.f);
    GetWorld()->GetTimerManager().SetTimer(SearchTimer,this,&UAIComponent::OnSearchFinished,5.f,false);
}
void UAIComponent::OnSearchFinished()
{
    if(CurrentTarget) return;
    if (CombatComponent)
    {
        CombatComponent->Disarm();
    }

    SetEnemyState(EEnemyState::EES_Patrol);
    PatrolTarget = SelectNextPatrolTarget();

    if (PatrolTarget)
    {
        MoveToActor(PatrolTarget);
    }
}
bool UAIComponent::IsTargetInAttackRange() const
{
    if(!CurrentTarget || !OwnerEnemy) return false;

    const double Distance = FVector::Dist(OwnerEnemy->GetActorLocation(),CurrentTarget->GetActorLocation());
    return Distance <= AttackRange;
}
bool UAIComponent::IsTargetTooFar() const
{
    if(!CurrentTarget || !OwnerEnemy) return true;

    const double Distance = FVector::Dist(OwnerEnemy->GetActorLocation(),CurrentTarget->GetActorLocation());

    return Distance > StopChaseRange;
}
bool UAIComponent::HasLineOfSightToTarget() const
{
    if(!CurrentTarget || !OwnerEnemy) return false;

    const FVector Start = OwnerEnemy->GetMesh()->GetSocketLocation(FName("head"));
    const FVector End = CurrentTarget->GetActorLocation();

    FHitResult Hit;

    FCollisionQueryParams Params;
    Params.AddIgnoredActor(OwnerEnemy);

    const bool bHit = GetWorld()->LineTraceSingleByChannel(
            Hit,
            Start,
            End,
            ECC_Visibility,
            Params
        );

    DrawDebugLine(GetWorld(),Start,End,bHit ? FColor::Green : FColor::Red,false,0.1f,0,3.f);

    return bHit && Hit.GetActor() == CurrentTarget;
}
void UAIComponent::CheckLineOfSight()
{
    if (!CurrentTarget)
    {
        GetWorld()->GetTimerManager().ClearTimer(LOSTimer);
        return;
    }

    if(EnemyState != EEnemyState::EES_Combat) return;

    if(IsTargetTooFar())
    {
        EndCombat();
        StartChasing();
        return;
    }

    if (HasLineOfSightToTarget())
    {
        BeginCombat();
    }
    else
    {
        EndCombat();
    }
}
void UAIComponent::CheckChaseDistance()
{
    if (!CurrentTarget)
    {
        GetWorld()->GetTimerManager().ClearTimer(ChaseTimer);
        return;
    }

    if (IsTargetTooFar())
    {
        LastKnownLocation = CurrentTarget->GetActorLocation();
        CurrentTarget = nullptr;
        GetWorld()->GetTimerManager().ClearTimer(ChaseTimer);

        if (EnemyController)
        {
            EnemyController->StopMovement();
            EnemyController->ClearFocus(EAIFocusPriority::Gameplay);
        }
        BeginSearch();
    }
    if(IsTargetInAttackRange())
    {
        EnterCombat();
    }
}
