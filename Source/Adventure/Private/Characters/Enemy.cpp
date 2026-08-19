#include "Characters/Enemy.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISense_Hearing.h"
#include "Perception/AISense_Damage.h"
#include "Items/Weapons/Weapon.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
    
    HolsterMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HolsterMesh"));
    HolsterMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    HolsterMesh->SetGenerateOverlapEvents(false);

    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Block);
    GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility,ECR_Ignore);

    GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    GetMesh()->SetCollisionResponseToAllChannels(ECR_Ignore);
    GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
    GetMesh()->SetGenerateOverlapEvents(false);
    
    AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComponent"));
}
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

    if (GetMesh() && HolsterMesh)
    {
        HolsterMesh->AttachToComponent(
            GetMesh(),
            FAttachmentTransformRules::SnapToTargetIncludingScale,
            TEXT("HolsterSocket")
        );
    }

    SetMovementSpeed(PatrolSpeed);

    if(AIPerceptionComponent)
    {
        AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(
            this,
            &AEnemy::OnTargetPerceptionUpdated
        );
    }

    //EnemyController = Cast<AAIController>(GetController());
    if(EnemyController)
    {
        EnemyController->GetPathFollowingComponent()->OnRequestFinished.AddUObject(
            this,
            &AEnemy::OnMoveCompleted
        );
    }
    if(PatrolTarget)
    {
        SetEnemyState(EEnemyState::EES_Patrol);
        MoveToActor(PatrolTarget);
    }
}
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
void AEnemy::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);

    EnemyController = Cast<AAIController>(NewController);
    InitializeAI();

    if(EnemyController)
    {
        UE_LOG(LogTemp, Warning, TEXT("Enemy Controller Possessed"));
    }
}
void AEnemy::MoveToActor(AActor* Target)
{
    if(EnemyController == nullptr || Target == nullptr) return;

    FAIMoveRequest MoveRequest;
    MoveRequest.SetGoalActor(Target);
    MoveRequest.SetAcceptanceRadius(15.f);
    EnemyController->MoveTo(MoveRequest);
}
AActor* AEnemy::SelectNextPatrolTarget()
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
bool AEnemy::IsTargetInRange(AActor* Target, double Radius) const
{
    if (!Target) return false;
    const double Distance = (Target->GetActorLocation() - GetActorLocation()).Size();
    return Distance <= Radius;
}
void AEnemy::SetEnemyState(EEnemyState NewState)
{
    if(EnemyState == NewState) return;

    const EEnemyState PreviousState = EnemyState;
    EnemyState = NewState;

    OnStateChanged(PreviousState, NewState);
}
void AEnemy::OnStateChanged(EEnemyState PreviousState, EEnemyState NewState)
{
    switch(NewState)
    {
        case EEnemyState::EES_Searching:
        case EEnemyState::EES_Investigating:
        case EEnemyState::EES_Combat:
        case EEnemyState::EES_Patrol:
            SetMovementSpeed(PatrolSpeed);
            break;

        case EEnemyState::EES_TakingCover:
        case EEnemyState::EES_Chasing:
            SetMovementSpeed(ChaseSpeed);
            break;

        case EEnemyState::EES_Idle:
        case EEnemyState::EES_Dead:
            SetMovementSpeed(0.f);
            break;

        default:
            break;
    }
}
bool AEnemy::HasLineOfSightToTarget()
{
    if (!CurrentTarget)
        return false;

    FVector Start = GetMesh()->GetSocketLocation(FName("head"));
    FVector End = CurrentTarget->GetActorLocation();

    FHitResult Hit;

    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);

    bool bHit = GetWorld()->LineTraceSingleByChannel(
        Hit,
        Start,
        End,
        ECC_Visibility,
        Params
    );

    DrawDebugLine(
        GetWorld(),
        Start,
        End,
        bHit ? FColor::Green : FColor::Red,
        false,
        0.1f,
        0,
        3.f
    );

    return bHit && Hit.GetActor() == CurrentTarget;
}
bool AEnemy::IsTargetInAttackRange() const
{
    if(!CurrentTarget) return false;
    return IsTargetInRange(CurrentTarget, AttackRange);
}
bool AEnemy::IsTargetTooFar() const
{
    if(!CurrentTarget) return true;
    return !IsTargetInRange(CurrentTarget, StopChaseRange);
}
void AEnemy::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
    if(!Result.IsSuccess()) return;

    if(EnemyState == EEnemyState::EES_Patrol)
    {
        PatrolTarget = SelectNextPatrolTarget();
        if(!PatrolTarget) return;

        const float WaitTime = FMath::RandRange(WaitMin, WaitMax);
        GetWorldTimerManager().SetTimer(PatrolTimer, this, &AEnemy::OnPatrolWaitFinished, WaitTime);
    }
    else if(EnemyState == EEnemyState::EES_Chasing)
    {
        if(IsTargetInAttackRange())
        {
            EnterCombat();
        }
        else
        {
            StartChasing();
        }
    }

}
void AEnemy::CheckLineOfSight()
{
    if(!CurrentTarget)
    {
        GetWorldTimerManager().ClearTimer(LOSTimer);
        return;
    }
    if(EnemyState != EEnemyState::EES_Combat) return;
    if(HasLineOfSightToTarget())
    {
        StartCombatAction();
    }
    else
    {
        StopCombatAction();
    }
}
void AEnemy::CheckChaseDistance()
{
    if(!CurrentTarget)
    {
        GetWorldTimerManager().ClearTimer(ChaseTimer);
        return;
    }

    if(IsTargetTooFar())
    {
        LastKnownLocation = CurrentTarget->GetActorLocation();

        CurrentTarget = nullptr;

        GetWorldTimerManager().ClearTimer(ChaseTimer);

        if(EnemyController)
        {
            EnemyController->StopMovement();
            EnemyController->ClearFocus(EAIFocusPriority::Gameplay);
        }

        BeginSearch();
    }
}
void AEnemy::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
    if(!Actor) return;

    if(Stimulus.Type == UAISense::GetSenseID<UAISense_Sight>())
    {
        if(Stimulus.WasSuccessfullySensed())
        {
            HandleSight(Actor);
        }
        else
        {
            HandleLostSight(Actor);
        }
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
void AEnemy::OnPatrolWaitFinished()
{
    MoveToActor(PatrolTarget);
}
void AEnemy::BeginFiring()
{
    if(CombatState != ECombatState::ECS_Aiming) return;

    GetWorldTimerManager().SetTimer(AttackTimer, this, &AEnemy::TryFireWeapon, 0.1f, true);
}
bool AEnemy::CanArm()
{
    return CombatState == ECombatState::ECS_Idle &&
        WeaponState == EWeaponState::EWS_Unarmed && EquippedWeapon;
}
bool AEnemy::CanDisarm()
{
    return CombatState == ECombatState::ECS_Idle &&
        WeaponState != EWeaponState::EWS_Unarmed;
}
bool AEnemy::CanReload() const
{
    return WeaponState == EWeaponState::EWS_Equipped && 
        Super::CanReload();
}
void AEnemy::Reload()
{
    if(!CanReload()) return;

    CombatState = ECombatState::ECS_Reloading;
    Super::Reload();
}
void AEnemy::FinishReloading_Implementation()
{
    Super::FinishReloading_Implementation();

    CombatState = ECombatState::ECS_Idle;
}
void AEnemy::InitializeAI()
{
    if(!EnemyController) return;

    if(PatrolTarget)
    {
        SetEnemyState(EEnemyState::EES_Patrol);
        MoveToActor(PatrolTarget);
    }
}
void AEnemy::Die()
{
    SetEnemyState(EEnemyState::EES_Dead);
    StopCombatAction();

    if(EnemyController)
    {
        EnemyController->StopMovement();
        EnemyController->ClearFocus(EAIFocusPriority::Gameplay);
    }

    GetCharacterMovement()->DisableMovement();
    AIPerceptionComponent->Deactivate();

    GetCapsuleComponent()->SetCollisionEnabled(
        ECollisionEnabled::NoCollision
    );
}
void AEnemy::EnterCombat()
{
    if(!CurrentTarget) return;

    GetWorldTimerManager().ClearTimer(ChaseTimer);
    SetEnemyState(EEnemyState::EES_Combat);
    if(EnemyController)
    {
        EnemyController->StopMovement();
    }
    CheckLineOfSight();
    
    if(!GetWorldTimerManager().IsTimerActive(LOSTimer))
    {
        GetWorldTimerManager().SetTimer(LOSTimer, this, &AEnemy::CheckLineOfSight, 0.1f, true);
    }
}
void AEnemy::StartCombatAction()
{
    if(CombatState != ECombatState::ECS_Idle) return;

    if(CanArm())
    {
        PlayEquipMontage(FName("Equip"));
        CombatState = ECombatState::ECS_EquippingWeapon;
        return;
    }

    if(WeaponState == EWeaponState::EWS_Equipped)
    {
        StartAiming();
    }
}
void AEnemy::TryFireWeapon()
{
    if(CombatState != ECombatState::ECS_Aiming) return;

    if(!EquippedWeapon) return;

    if(!CanFire())
    {
        TryReload();
        return;
    }
    Fire();
}
void AEnemy::TryReload()
{
    if(CombatState == ECombatState::ECS_Reloading ||
       CombatState == ECombatState::ECS_EquippingWeapon) return;

    if(CanReload())
    {
        GetWorldTimerManager().ClearTimer(AttackTimer);
        Reload();
    }
}
void AEnemy::StopCombatAction()
{
    GetWorldTimerManager().ClearTimer(AttackTimer);
    GetWorldTimerManager().ClearTimer(AimTimer);
    GetWorldTimerManager().ClearTimer(LOSTimer);

    switch (CombatState)
    {
        case ECombatState::ECS_Aiming:
        {
            CombatState = ECombatState::ECS_Idle;
            break;
        }

        case ECombatState::ECS_EquippingWeapon:
        case ECombatState::ECS_Reloading:
        case ECombatState::ECS_Idle:
        default:
        {
            break;
        }
    }
}
void AEnemy::StartAiming()
{
    CombatState = ECombatState::ECS_Aiming;
    GetWorldTimerManager().SetTimer(AimTimer, this, &AEnemy::BeginFiring, AimDelay, false);
}
void AEnemy::StopAiming()
{
}
void AEnemy::StartChasing()
{
    if(!CurrentTarget) return;

    GetWorldTimerManager().ClearTimer(PatrolTimer);
    SetEnemyState(EEnemyState::EES_Chasing);
    SetMovementSpeed(ChaseSpeed);
    MoveToActor(CurrentTarget);

    if(!GetWorldTimerManager().IsTimerActive(ChaseTimer))
    {
        GetWorldTimerManager().SetTimer(
            ChaseTimer,
            this,
            &AEnemy::CheckChaseDistance,
            0.2f,
            true
        );
    }
}
void AEnemy::BeginSearch()
{
    SetEnemyState(EEnemyState::EES_Searching);
    EnemyController->MoveToLocation(LastKnownLocation, 50.f);

    GetWorldTimerManager().SetTimer(SearchTimer, this, &AEnemy::OnSearchFinished, 5.f, false);
}
void AEnemy::OnSearchFinished()
{
    if(CurrentTarget) return;

    if(CanDisarm())
    {
        PlayEquipMontage(FName("Unequip"));
        CombatState = ECombatState::ECS_EquippingWeapon;
    }

    SetEnemyState(EEnemyState::EES_Patrol);
    PatrolTarget = SelectNextPatrolTarget();

    if(PatrolTarget)
    {
        MoveToActor(PatrolTarget);
    }
}
void AEnemy::HandleSight(AActor* DetectedActor)
{
    if(!DetectedActor) return;

    EnemyDetectionType = EEnemyDetectionType::EDT_Sight;
    CurrentTarget = DetectedActor;
    LastKnownLocation = DetectedActor->GetActorLocation();
    if(EnemyController)
    {
        EnemyController->SetFocus(CurrentTarget);
    }
    StartChasing();
}
void AEnemy::HandleLostSight(AActor* DetectedActor)
{
    if(DetectedActor != CurrentTarget) return;

    LastKnownLocation = DetectedActor->GetActorLocation();
    CurrentTarget = nullptr;
    SetEnemyState(EEnemyState::EES_Searching);
    StopCombatAction();
    if(EnemyController)
    {
        EnemyController->ClearFocus(EAIFocusPriority::Gameplay);
    }
    BeginSearch();
}
void AEnemy::HandleHearing(const FVector& Location)
{
    EnemyDetectionType = EEnemyDetectionType::EDT_Hearing;
    LastKnownLocation = Location;
    SetEnemyState(EEnemyState::EES_Investigating);
    if(EnemyController)
    {
        EnemyController->ClearFocus(EAIFocusPriority::Gameplay);
        EnemyController->MoveToLocation(LastKnownLocation, 50.f);
    }
}
void AEnemy::HandleDamage(AActor* DamageCauser)
{
    if(!DamageCauser) return;
    CurrentTarget = DamageCauser;
    LastKnownLocation = DamageCauser->GetActorLocation();
    EnemyDetectionType = EEnemyDetectionType::EDT_Damage;
    GetWorldTimerManager().ClearTimer(PatrolTimer);

    if(EnemyController)
    {
        EnemyController->SetFocus(CurrentTarget);
    }

    if(IsTargetInAttackRange())
    {
        EnterCombat();
    }
    else
    {
        StartChasing();
    }
}
