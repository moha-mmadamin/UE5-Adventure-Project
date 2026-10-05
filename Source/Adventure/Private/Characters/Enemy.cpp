#include "Characters/Enemy.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Weapons/BaseWeapon.h"
#include "Components/CapsuleComponent.h"
#include "Components/Combat/CombatComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/Health/HealthComponent.h"
#include "UI/HealthBarWidget.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "UI/EnemyHealthBar.h"
#include "AI/EnemyAIController.h"

AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

    GetCharacterMovement()->bUseControllerDesiredRotation = true;
	GetCharacterMovement()->bOrientRotationToMovement = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;
    
    HolsterMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HolsterMesh"));
    HolsterMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    HolsterMesh->SetGenerateOverlapEvents(false);

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

    if (HealthComponent)
    {
        HealthComponent->OnHealthChanged.AddDynamic(this, &AEnemy::ShowHealthBar);
        HealthComponent->OnDeath.AddDynamic(this, &AEnemy::Die);
    }

    if (HealthWidget)
    {
        UUserWidget* Widget = HealthWidget->GetUserWidgetObject();
        if (Widget)
        {
            EnemyHealthBar = Cast<UEnemyHealthBar>(Widget);
            if (EnemyHealthBar)
            {
                EnemyHealthBar->SetVisibility(ESlateVisibility::Hidden);
                EnemyHealthBar->SetHealthPercentage(HealthComponent->GetHealthPercent());
                HealthComponent->OnHealthChanged.AddDynamic(EnemyHealthBar,&UEnemyHealthBar::SetHealthPercentage);
            }
        }
    }
}
void AEnemy::Reload()
{
    ABaseWeapon* Weapon = CombatComponent->GetEquippedWeapon();
    if(!Weapon) return;
    if(Weapon->GetCurrentAmmo() <= 0)
    {
        if(CombatComponent->GetCombatState() ==ECombatState::ECS_Aiming)
        {
            CombatComponent->StopAiming();
        }
        CombatComponent->Reload();
        return;
    }
    return;
}
void AEnemy::ShowHealthBar(float HealthPercent)
{
    if(!IsValid(this) || !EnemyHealthBar) return;

	EnemyHealthBar->SetVisibility(ESlateVisibility::Visible);

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(HealthBarHideTimer);
		World->GetTimerManager().SetTimer(
			HealthBarHideTimer,
			this,
			&AEnemy::HideHealthBar,
			HealthBarVisibleDuration,
			false
		);
	}
}
void AEnemy::HideHealthBar()
{
    if (!IsValid(this) || !EnemyHealthBar) return;

    EnemyHealthBar->SetVisibility(ESlateVisibility::Hidden);
}
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
void AEnemy::Die()
{
    if(UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(HealthBarHideTimer);
        //World->GetTimerManager().ClearTimer(LookAroundTimer);
	}

    if (CombatComponent)
    {
        ABaseWeapon* EquippedWeapon = CombatComponent->GetEquippedWeapon();

        if (EquippedWeapon)
        {
            EquippedWeapon->SetLifeSpan(5.0f);
        }
    }

    Super::Die();

    if(HealthWidget)
    {
       HealthWidget->SetVisibility(false);
    }

    EnemyHealthBar = nullptr;
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    SetLifeSpan(5.0f);
}