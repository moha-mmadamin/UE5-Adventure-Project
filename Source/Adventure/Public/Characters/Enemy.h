#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "EnemyTypes.h"
#include "CharacterTypes.h"
#include "Perception/AIPerceptionTypes.h"
#include "Components/StaticMeshComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "Enemy.generated.h"

class UAIPerceptionComponent;
class AAIController;
class AWeapon;
class UAnimMontage;

UCLASS()
class ADVENTURE_API AEnemy : public ABaseCharacter
{
	GENERATED_BODY()

public:
	AEnemy();
	virtual void Tick(float DeltaTime) override;
	virtual void PossessedBy(AController* NewController) override;

protected:
	virtual void BeginPlay() override;
	void MoveToActor(AActor* Target);
	AActor* SelectNextPatrolTarget();
	bool IsTargetInRange(AActor* Target, double Radius) const;

	/*
	AI senses
	*/

	UFUNCTION(BlueprintCallable, Category="AI")
	void HandleSight(AActor* DetectedActor);

	UFUNCTION(BlueprintCallable, Category="AI")
	void HandleLostSight(AActor* DetectedActor);

	UFUNCTION(BlueprintCallable, Category = "AI")
	void HandleHearing(const FVector& Location);

	UFUNCTION(BlueprintCallable, Category="AI")
	void HandleDamage(AActor* DamageCauser);

private:
	virtual bool CanArm() override;
	virtual bool CanDisarm() override;
	virtual bool CanReload() const override;
	virtual void Reload() override;
	virtual void FinishReloading_Implementation() override;
	void InitializeAI();
	void OnPatrolWaitFinished();
	void BeginFiring();
	void Die();
	void EnterCombat();
	void StartCombatAction();
	void StopCombatAction();
	void StartAiming();
	void StopAiming();
	void StartChasing();
	void TryFireWeapon();
	void TryReload();
	void BeginSearch();
	void OnSearchFinished();
	void SetEnemyState(EEnemyState NewState);
	void OnStateChanged(EEnemyState PreviousState, EEnemyState NewState);
	bool HasLineOfSightToTarget();
	bool IsTargetInAttackRange() const;
	bool IsTargetTooFar() const;
	bool CanOverrideDetection(EEnemyDetectionType NewDetection) const;
	void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result);
	void CheckLineOfSight();
	void CheckChaseDistance();

	UFUNCTION()
	void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	UAIPerceptionComponent* AIPerceptionComponent;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	EEnemyState EnemyState = EEnemyState::EES_Idle;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	EEnemyDetectionType EnemyDetectionType = EEnemyDetectionType::EDT_None;

	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	AActor* CurrentTarget = nullptr;

	UPROPERTY()
	FVector LastKnownLocation = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, Category="Combat")
	float AttackRange = 1000.f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float StopChaseRange = 1500.f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float AimDelay = 0.5f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment", meta=(AllowPrivateAccess="true"))
	UStaticMeshComponent* HolsterMesh;



	/*
	Navigation
	*/

	UPROPERTY()
	AAIController* EnemyController;

	//Current patrol target
	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	AActor* PatrolTarget;

	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	TArray<AActor*> PatrolTargets;

	UPROPERTY(EditAnywhere)
	double PatrolRadius = 200.f;

	FTimerHandle PatrolTimer;
	FTimerHandle SearchTimer;
	FTimerHandle AimTimer;
	FTimerHandle AttackTimer;
	FTimerHandle LOSTimer;
	FTimerHandle ChaseTimer;


	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float WaitMin = 5.f;

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float WaitMax = 10.f;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float PatrolSpeed = 100.f;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float ChaseSpeed = 400.f;
};
