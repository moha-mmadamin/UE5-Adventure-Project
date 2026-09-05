#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Characters/EnemyTypes.h"
#include "Navigation/PathFollowingComponent.h"
#include "AIController.h"
#include "AIComponent.generated.h"

class AEnemy;
class AActor;
class UCombatComponent;
class UPerceptionComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ADVENTURE_API UAIComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UAIComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
    void InitializeAI();
    void SetEnemyState(EEnemyState NewState);
    void StopAI();

protected:
	virtual void BeginPlay() override;

private:

    /*
    Perception callbacks
    */

    void HandleSight(AActor* DetectedActor);
    void HandleLostSight(AActor* DetectedActor);
    void HandleHearing(const FVector& Location);
    void HandleDamage(AActor* DamageCauser);

    /*
    State
    */

    void OnStateChanged(EEnemyState PreviousState, EEnemyState NewState);

    /*
    Combat / Decision
    */

    void StartChasing();

    void EnterCombat();
    void BeginCombat();
    void EndCombat();

    void BeginSearch();
    void OnSearchFinished();

    bool IsTargetInAttackRange() const;
    bool IsTargetTooFar() const;

    bool HasLineOfSightToTarget() const;

    void CheckLineOfSight();
    void CheckChaseDistance();

    /*
    Navigation
    */

    void MoveToActor(AActor* Target);
    AActor* SelectNextPatrolTarget();
    void OnMoveCompleted(FAIRequestID RequestID,const FPathFollowingResult& Result);
    void OnPatrolWaitFinished();

private:

    UPROPERTY()
    AEnemy* OwnerEnemy;

    UPROPERTY()
    AAIController* EnemyController;

    UPROPERTY()
    UCombatComponent* CombatComponent;

    UPROPERTY()
    UPerceptionComponent* PerceptionComponent;

    /*
    State
    */

    UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
    EEnemyState EnemyState = EEnemyState::EES_Idle;

    UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
    EEnemyDetectionType EnemyDetectionType = EEnemyDetectionType::EDT_None;

    /*
    Target
    */

    UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
    AActor* CurrentTarget = nullptr;

    UPROPERTY()
    FVector LastKnownLocation = FVector::ZeroVector;

    /*
    Combat
    */

    UPROPERTY(EditAnywhere, Category="Combat")
    float AttackRange = 1000.f;

    UPROPERTY(EditAnywhere, Category="Combat")
    float StopChaseRange = 1500.f;

    UPROPERTY(EditAnywhere, Category="Combat")
    float ChaseAcceptanceRadius = 800.f;

    /*
    Navigation
    */

    UPROPERTY(EditInstanceOnly, Category="AI Navigation")
    AActor* PatrolTarget;

    UPROPERTY(EditInstanceOnly, Category="AI Navigation")
    TArray<AActor*> PatrolTargets;

    UPROPERTY(EditAnywhere, Category="AI Navigation")
    float PatrolRadius = 200.f;

    UPROPERTY(EditAnywhere, Category="AI Navigation")
    float WaitMin = 5.f;

    UPROPERTY(EditAnywhere, Category="AI Navigation")
    float WaitMax = 10.f;

    /*
    Movement
    */

    UPROPERTY(EditAnywhere, Category="Movement")
    float PatrolSpeed = 100.f;

    UPROPERTY(EditAnywhere, Category="Movement")
    float ChaseSpeed = 400.f;

    /*
    Timers
    */

    FTimerHandle PatrolTimer;
    FTimerHandle SearchTimer;
    FTimerHandle LOSTimer;
    FTimerHandle ChaseTimer;		
};
