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
        MoveToActor(PatrolTarget);
    }
}
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
    
    if(EnemyState == EEnemyState::EES_Patrol)
    {
        UpdatePatrol();
    }
    else if (EnemyState == EEnemyState::EES_Chasing ||
             EnemyState == EEnemyState::EES_Combat)
    {
        UpdateCombat();
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
bool AEnemy::IsTargetInRange(AActor* Target, double Radius)
{
    if (Target == nullptr) return false;
    const double DistanceToTarget = (Target->GetActorLocation() - GetActorLocation()).Size();
    return DistanceToTarget <= Radius;
}
void AEnemy::UpdatePatrol()
{
    if(!PatrolTarget) return;
    if (GetWorldTimerManager().IsTimerActive(PatrolTimer)) return;

    if(IsTargetInRange(PatrolTarget, PatrolRadius))
    {
        PatrolTarget = SelectNextPatrolTarget();
        const float WaitTime = FMath::RandRange(WaitMin, WaitMax);
        GetWorldTimerManager().SetTimer(PatrolTimer, this, &AEnemy::OnPatrolWaitFinished, WaitTime);
    }
}
void AEnemy::UpdateCombat()
{
    if(!CurrentTarget)
    {
        StopCombatAction();
        return;
    }

    const float Distance = FVector::Dist(GetActorLocation(), CurrentTarget->GetActorLocation());

    // Target is too far
    if(Distance > StopChaseRange)
    {
        StopCombatAction();

        LastKnownLocation = CurrentTarget->GetActorLocation();
        CurrentTarget = nullptr;

        SetEnemyState(EEnemyState::EES_Searching);

        if(EnemyController)
        {
            EnemyController->StopMovement();
            EnemyController->ClearFocus(EAIFocusPriority::Gameplay);
        }
        BeginSearch();
        return;
    }

    // Target is outside attack range
    if (Distance > AttackRange)
    {
        StopCombatAction();
        SetEnemyState(EEnemyState::EES_Chasing);
        MoveToActor(CurrentTarget);
        return;
    }
    // Target is inside attack range
    SetEnemyState(EEnemyState::EES_Combat);
    if(EnemyController)
    {
        EnemyController->StopMovement();
    }
    if(HasLineOfSightToTarget())
    {
        StartCombatAction();
    }
    else if(WeaponState == EWeaponState::EWS_Equipped)
    {
        StopCombatAction();
    }
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
    if(ActionState != EActionState::EAS_Aiming) return;

    GetWorldTimerManager().SetTimer(AttackTimer, this, &AEnemy::TryFireWeapon, 0.2f, true);
}
bool AEnemy::CanArm()
{
    return ActionState == EActionState::EAS_Unoccupied &&
        WeaponState == EWeaponState::EWS_Unarmed && EquippedWeapon;
}
bool AEnemy::CanDisarm()
{
    return ActionState == EActionState::EAS_Unoccupied &&
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

    ActionState = EActionState::EAS_Reloading;
    Super::Reload();
}
void AEnemy::FinishEquipping_Implementation()
{
    Super::FinishEquipping_Implementation();

    if (WeaponState == EWeaponState::EWS_Unarmed)
    {
        WeaponState = EWeaponState::EWS_Equipped;
        ActionState = EActionState::EAS_Unoccupied;
    }
    else
    {
        WeaponState = EWeaponState::EWS_Unarmed;
        ActionState = EActionState::EAS_Unoccupied;
    }
    
}
void AEnemy::FinishReloading_Implementation()
{
    Super::FinishReloading_Implementation();

    ActionState = EActionState::EAS_Unoccupied;
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
void AEnemy::StartCombatAction()
{
    if(ActionState == EActionState::EAS_EquippingWeapon ||
       ActionState == EActionState::EAS_Reloading) return;

    if(CanArm())
    {
        PlayEquipMontage(FName("Equip"));
        ActionState = EActionState::EAS_EquippingWeapon;
        return;
    }

    if(WeaponState == EWeaponState::EWS_Equipped && ActionState == EActionState::EAS_Unoccupied)
    {
        ActionState = EActionState::EAS_Aiming;
        GetWorldTimerManager().SetTimer(AimTimer, this, &AEnemy::BeginFiring, AimDelay, false);
    }
}
void AEnemy::TryFireWeapon()
{
    if(ActionState != EActionState::EAS_Aiming) return;

    if(EquippedWeapon->GetCurrentAmmo() <= 0)
    {
        TryReload();
        return;
    }

    if(CanFire())
    {
        Fire();
    }
}
void AEnemy::TryReload()
{
    if(ActionState == EActionState::EAS_Reloading ||
       ActionState == EActionState::EAS_EquippingWeapon) return;

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

    if(ActionState == EActionState::EAS_EquippingWeapon || ActionState == EActionState::EAS_Reloading) return;

    if(ActionState == EActionState::EAS_Aiming || ActionState == EActionState::EAS_Shooting)
    {
        ActionState = EActionState::EAS_Unoccupied;
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
        ActionState = EActionState::EAS_EquippingWeapon;
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
    SetEnemyState(EEnemyState::EES_Combat);
    GetWorldTimerManager().ClearTimer(PatrolTimer);

    if(EnemyController)
    {
        EnemyController->SetFocus(CurrentTarget);
    }
    UpdateCombat();
}
