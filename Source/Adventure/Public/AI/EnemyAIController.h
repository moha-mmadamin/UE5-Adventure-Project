#pragma once

#include "CoreMinimal.h"
#include "Characters/EnemyTypes.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "EnemyAIController.generated.h"

UCLASS()
class ADVENTURE_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()

public:
	AEnemyAIController();

    UFUNCTION(BlueprintCallable)
    void SetEnemyState(EEnemyState NewState);

    UFUNCTION(BlueprintCallable)
    void LookAround();

    UFUNCTION(BlueprintCallable)
    bool IsLookingAround() const;

protected:
    virtual void BeginPlay() override;

    UFUNCTION()
    void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

private:

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI|Perception", meta = (AllowPrivateAccess = "true"))
    UAIPerceptionComponent* AIPerceptionComponent;

    void HandleSight(AActor* DetectedActor, const FAIStimulus& Stimulus);
    void HandleHearing(const FAIStimulus& Stimulus);
    void HandleDamage(AActor* DamageCauser, const FAIStimulus& Stimulus);

    void SetTargetActor(AActor* NewTarget);
    void SetLastKnownLocation(const FVector& Location);
    void ClearTargetActor();

    /*
    State
    */

    void OnStateChanged(EEnemyState PreviousState, EEnemyState NewState);

    UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
    EEnemyState EnemyState = EEnemyState::EES_Idle;

    /*
    Movement
    */

    UPROPERTY(EditAnywhere, Category="Movement")
    float PatrolSpeed = 100.f;

    UPROPERTY(EditAnywhere, Category="Movement")
    float ChaseSpeed = 400.f;

    /*
    Navigation
    */

    AActor* SelectNextPatrolTarget();

    UFUNCTION(BlueprintCallable)
    void SetNextPatrolTargetOnBlackboard();

    UPROPERTY()
    AActor* PatrolTarget;

    UPROPERTY(EditAnywhere, Category="AI Navigation")
    float PatrolRadius = 200.f;

    /*
    Look Around
    */

    void FinishLookAround();

    int32 LookAroundStep = 0;

    float LookAroundStartYaw = 0.f;

    UPROPERTY(EditAnywhere, Category = "AI|Investigation")
    float LookAroundDelay = 1.2f;

    UPROPERTY(EditAnywhere, Category = "AI|Investigation")
    float LookAroundAngle = 60.f;

    /*
    Timers
    */
	
    FTimerHandle LookAroundTimer;

    bool bHasSightTarget = false;

};
