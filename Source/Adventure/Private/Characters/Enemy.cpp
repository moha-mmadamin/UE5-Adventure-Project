#include "Characters/Enemy.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "Items/Weapons/Weapon.h"
#include "Components/CapsuleComponent.h"
#include "Components/AI/AIComponent.h"
#include "Components/Perception/PerceptionComponent.h"
#include "Components/Combat/CombatComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"

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

    AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComponent"));
    PerceptionComponent = CreateDefaultSubobject<UPerceptionComponent>(TEXT("PerceptionComponent"));
    AIComponent = CreateDefaultSubobject<UAIComponent>(TEXT("AIComponent"));

    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Block);
    GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility,ECR_Ignore);
    GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera,ECR_Ignore);

    GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    GetMesh()->SetCollisionResponseToAllChannels(ECR_Ignore);
    GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
    GetMesh()->SetGenerateOverlapEvents(false);
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
    EnemyController = Cast<AAIController>(GetController());
}
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
void AEnemy::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);

    EnemyController = Cast<AAIController>(NewController);

    UE_LOG(
        LogTemp,
        Warning,
        TEXT("Enemy possessed by: %s"),
        *GetNameSafe(NewController)
    );

    if(AIComponent)
    {
        AIComponent->InitializeAI();
    }
}
void AEnemy::DeactivatePerception()
{
    if (AIPerceptionComponent)
    {
        AIPerceptionComponent->Deactivate();
    }

    if (PerceptionComponent)
    {
        PerceptionComponent->Deactivate();
    }
}
void AEnemy::BeginFiring()
{
    if(CombatComponent->GetCombatState() != ECombatState::ECS_Aiming) return;

    GetWorldTimerManager().SetTimer(AttackTimer, this, &AEnemy::TryFire, 0.1f, true);
}
void AEnemy::Die()
{
    if(!AIComponent) return;
    
    AIComponent->SetEnemyState(EEnemyState::EES_Dead);
    CombatComponent->StopAiming();

    if(EnemyController)
    {
        EnemyController->StopMovement();
        EnemyController->ClearFocus(EAIFocusPriority::Gameplay);
    }

    GetCharacterMovement()->DisableMovement();
    DeactivatePerception();

    GetCapsuleComponent()->SetCollisionEnabled(
        ECollisionEnabled::NoCollision
    );
}
void AEnemy::TryFire()
{
    if(!CombatComponent) return;

    if(CombatComponent->CanFire())
    {
        CombatComponent->Fire();
    }
    else
    {
        TryReload();
    }
}
void AEnemy::TryReload()
{
    if(!CombatComponent) return;

    //if(!CombatComponent->CanReload()) return;
    GetWorldTimerManager().ClearTimer(AttackTimer);

    CombatComponent->Reload();
}
void AEnemy::StartAiming()
{
    if(!CombatComponent) return;
    CombatComponent->StartAiming();

    GetWorldTimerManager().SetTimer(AimTimer, this, &AEnemy::BeginFiring, AimDelay, false);
}
void AEnemy::StopAiming()
{
    if(!CombatComponent) return;
    CombatComponent->StopAiming();
}