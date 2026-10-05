#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "CharacterTypes.h"
#include "UI/HealthBarWidget.h"
#include "Enemy.generated.h"

// Forward Declarations
class AEnemyAIController;
class UStaticMeshComponent;
class UAIComponent;
class UHealthComponent;
class UWidgetComponent;
class UEnemyHealthBar;
class UBaseWeapon;


UCLASS()
class ADVENTURE_API AEnemy : public ABaseCharacter
{
	GENERATED_BODY()

public:
	AEnemy();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

    /*
    Behavior tree Combat section
    */

    UFUNCTION(BlueprintCallable)
    void Reload();

private:
    void HideHealthBar();

    UFUNCTION()
    void ShowHealthBar(float HealthPercent);

    UFUNCTION()
    virtual void Die() override;

    // Controller
    UPROPERTY()
    AEnemyAIController* EnemyController;

    // Equipment
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment", meta = (AllowPrivateAccess = "true"))
    UStaticMeshComponent* HolsterMesh;

    //Navigation
    UPROPERTY(EditInstanceOnly, Category="AI Navigation")
    TArray<AActor*> PatrolTargets;

    /*
    Combat Settings
    */

    UPROPERTY(EditAnywhere, Category = "Combat")
    float AttackRange = 1000.f;

    UPROPERTY(EditAnywhere, Category = "Combat")
    float AimDelay = 0.5f;

    UPROPERTY(EditAnywhere, Category="Combat")
    float StopChaseRange = 1500.f;

    UPROPERTY(EditAnywhere, Category="Combat")
    float ChaseAcceptanceRadius = 800.f;

    /*
    Health
    */

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health", meta = (AllowPrivateAccess = "true"))
    UHealthComponent* HealthComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health", meta = (AllowPrivateAccess = "true"))
    UWidgetComponent* HealthWidget;

    UPROPERTY()
    UEnemyHealthBar* EnemyHealthBar;

    /*
    Timers
    */

    FTimerHandle AimTimer;
    FTimerHandle AttackTimer;
    FTimerHandle HealthBarHideTimer;
    FTimerHandle PatrolWaitTimer;

    UPROPERTY(EditAnywhere, Category = "UI")
    float HealthBarVisibleDuration = 5.0f;

    /*
    Timers
    */

    FTimerHandle PatrolTimer;
    FTimerHandle SearchTimer;
    FTimerHandle LOSTimer;
    FTimerHandle ChaseTimer;

public:
    const TArray<AActor*>& GetPatrolTargets() const {return PatrolTargets;}
};