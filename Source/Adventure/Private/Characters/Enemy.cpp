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
    //HolsterMesh->SetupAttachment(GetMesh(), TEXT("HolsterSocket"));
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

    EnemyController = Cast<AAIController>(GetController());
    if(PatrolTarget)
    {
        SetEnemyState(EEnemyState::EES_Patrol);
        MoveToTarget(PatrolTarget);
    }
}
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
    
    if(EnemyState == EEnemyState::EES_Patrol)
    {
        CheckPatrolTarget();
    }
    else if (EnemyState == EEnemyState::EES_Chasing ||
             EnemyState == EEnemyState::EES_Combat)
    {
        UpdateCombat();
    }
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
void AEnemy::CheckPatrolTarget()
{
    if(!PatrolTarget) return;
    if (GetWorldTimerManager().IsTimerActive(PatrolTimer)) return;

    if(InTargetRange(PatrolTarget, PatrolRadius))
    {
        PatrolTarget = ChoosePatrolTarget();
        const float WaitTime = FMath::RandRange(WaitMin, WaitMax);
        GetWorldTimerManager().SetTimer(PatrolTimer, this, &AEnemy::PatrolTimerFinished, WaitTime);
    }
}
void AEnemy::UpdateCombat()
{
    if(!CurrentTarget)
    {
        StopAttack();
        return;
    }

    const float Distance = FVector::Dist(GetActorLocation(), CurrentTarget->GetActorLocation());

    // Target is too far
    if(Distance > StopChaseRange)
    {
        StopAttack();

        LastKnownLocation = CurrentTarget->GetActorLocation();
        CurrentTarget = nullptr;

        SetEnemyState(EEnemyState::EES_Searching);

        if(EnemyController)
        {
            EnemyController->StopMovement();
            EnemyController->ClearFocus(EAIFocusPriority::Gameplay);
        }
        StartSearching();
        return;
    }

    // Target is outside attack range
    if (Distance > AttackRange)
    {
        StopAttack();
        SetEnemyState(EEnemyState::EES_Chasing);
        MoveToTarget(CurrentTarget);
        return;
    }
    // Target is inside attack range
    SetEnemyState(EEnemyState::EES_Combat);
    if(EnemyController)
    {
        EnemyController->StopMovement();
    }
    if(CanAttackTarget())
    {
        StartAttack();
    }
    else
    {
        StopAttack();
    }
}
void AEnemy::InvestigateLastKnownLocation()
{
    SetEnemyState(EEnemyState::EES_Investigating);

    EnemyController->MoveToLocation(LastKnownLocation, 50.f);
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
bool AEnemy::CanAttackTarget()
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
void AEnemy::PatrolTimerFinished()
{
    MoveToTarget(PatrolTarget);
}
void AEnemy::Die()
{
    SetEnemyState(EEnemyState::EES_Dead);
    StopAttack();

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
void AEnemy::StartAttack()
{
    if(GetWorldTimerManager().IsTimerActive(AttackTimer)) return;

    WeaponState = EWeaponState::EWS_Equipped;
    ActionState = EActionState::EAS_Aiming;
    PlayEquipMontage(FName("Equip"));
    Attack();

    GetWorldTimerManager().SetTimer(AttackTimer, this, &AEnemy::Attack, 0.2f, true);
}
void AEnemy::Attack()
{
    if(!EquippedWeapon) return;

    if(CanFire())
    {
        Fire();
    }
}
void AEnemy::StopAttack()
{
    GetWorldTimerManager().ClearTimer(AttackTimer);
    WeaponState = EWeaponState::EWS_Unarmed;
    ActionState = EActionState::EAS_Unoccupied;
    PlayEquipMontage(FName("Unequip"));
}
void AEnemy::StartSearching()
{
    SetEnemyState(EEnemyState::EES_Searching);
    EnemyController->MoveToLocation(LastKnownLocation, 50.f);

    GetWorldTimerManager().SetTimer(SearchTimer, this, &AEnemy::FinishSearching, 5.f, false);
}
void AEnemy::FinishSearching()
{
    if(CurrentTarget) return;

    SetEnemyState(EEnemyState::EES_Patrol);

    PatrolTarget = ChoosePatrolTarget();

    if(PatrolTarget)
    {
        MoveToTarget(PatrolTarget);
    }
}
void AEnemy::HandleSight(AActor* DetectedActor)
{
    if(!DetectedActor) return;

    EnemyDetectionType = EEnemyDetectionType::EDT_Sight;
    CurrentTarget = DetectedActor;
    LastKnownLocation = DetectedActor->GetActorLocation();
    SetEnemyState(EEnemyState::EES_Chasing);
    GetWorldTimerManager().ClearTimer(PatrolTimer);
    if(EnemyController)
    {
        EnemyController->SetFocus(CurrentTarget);
    }
    UpdateCombat();
    //UE_LOG(LogTemp, Warning, TEXT("Enemy detected %s by sight."), *DetectedActor->GetName());
}
void AEnemy::HandleLostSight(AActor* DetectedActor)
{
    if(DetectedActor != CurrentTarget) return;

    LastKnownLocation = DetectedActor->GetActorLocation();
    CurrentTarget = nullptr;
    SetEnemyState(EEnemyState::EES_Searching);
    StopAttack();
    if(EnemyController)
    {
        EnemyController->ClearFocus(EAIFocusPriority::Gameplay);
    }
    StartSearching();
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
    SetEnemyState(EEnemyState::EES_Combat);
    GetWorldTimerManager().ClearTimer(PatrolTimer);

    if(EnemyController)
    {
        EnemyController->SetFocus(CurrentTarget);
    }
    UpdateCombat();
}
