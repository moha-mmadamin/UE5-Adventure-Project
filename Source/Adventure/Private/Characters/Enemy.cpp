#include "Characters/Enemy.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Items/Weapons/Weapon.h"
#include "Components/CapsuleComponent.h"
#include "Components/AI/AIComponent.h"
#include "Components/Perception/PerceptionComponent.h"
#include "Components/Combat/CombatComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/Health/HealthComponent.h"
#include "UI/HealthBarWidget.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "UI/EnemyHealthBar.h"

AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	GetCharacterMovement()->bOrientRotationToMovement = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;
    
    HolsterMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HolsterMesh"));
    HolsterMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    HolsterMesh->SetGenerateOverlapEvents(false);

    AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComponent"));
    PerceptionComponent = CreateDefaultSubobject<UPerceptionComponent>(TEXT("PerceptionComponent"));
    AIComponent = CreateDefaultSubobject<UAIComponent>(TEXT("AIComponent"));
    HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
    HealthWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthWidget"));

    HealthWidget->SetupAttachment(GetRootComponent());
    HealthWidget->SetWidgetSpace(EWidgetSpace::Screen);
    HealthWidget->SetDrawSize(FVector2D(200.f, 50.f));
    HealthWidget->SetRelativeLocation(FVector(0.f, 0.f, 200.f));

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
    if (HealthWidget)
    {
        UUserWidget* Widget = HealthWidget->GetUserWidgetObject();
        if (Widget)
        {
            EnemyHealthBar = Cast<UEnemyHealthBar>(Widget);
            if (EnemyHealthBar)
            {
                EnemyHealthBar->SetHealthPercentage(HealthComponent->GetHealthPercent());
                HealthComponent->OnHealthChanged.AddDynamic(EnemyHealthBar,&UEnemyHealthBar::SetHealthPercentage);
            }
        }
    }
    if(HealthComponent)
    {
        HealthComponent->OnDeath.AddDynamic(this, &AEnemy::Die);
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

    if(AIComponent)
    {
        AIComponent->InitializeAI();
    }
}
void AEnemy::Die()
{
    Super::Die();

    if(CombatComponent)
    {
        AWeapon* Weapon = CombatComponent->GetEquippedWeapon();
        if(Weapon)
        {
            Weapon->SetLifeSpan(5.0f);
        }
    }    
    if(HealthWidget)
    {
       HealthWidget->SetVisibility(false);
    }
    if(AIComponent)
    {
        AIComponent->StopAI();
    }
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    SetLifeSpan(5.0f);
}