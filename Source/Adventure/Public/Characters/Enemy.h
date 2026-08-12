#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "EnemyTypes.h"
#include "CharacterTypes.h"
#include "Perception/AIPerceptionTypes.h"
#include "Components/StaticMeshComponent.h"
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

protected:
	virtual void BeginPlay() override;
	void MoveToTarget(AActor* Target);
	AActor* ChoosePatrolTarget();
	bool InTargetRange(AActor* Target, double Radius);

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
	void Die();
	void StartAttack();
	void StopAttack();
	void Attack();
	void StartSearching();
	void FinishSearching();
	void CheckPatrolTarget();
	void UpdateCombat();
	void InvestigateLastKnownLocation();
	void SetEnemyState(EEnemyState NewState);
	void OnStateChanged(EEnemyState PreviousState, EEnemyState NewState);
	bool CanAttackTarget();

	UFUNCTION()
	void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	UAIPerceptionComponent* AIPerceptionComponent;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	EEnemyState EnemyState = EEnemyState::EES_Idle;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	EEnemyDetectionType EnemyDetectionType = EEnemyDetectionType::EDT_None;

	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	EWeaponState WeaponState = EWeaponState::EWS_Unarmed;

	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	EActionState ActionState = EActionState::EAS_Unoccupied;

	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	AActor* CurrentTarget = nullptr;

	UPROPERTY()
	FVector LastKnownLocation = FVector::ZeroVector;

	UPROPERTY(EditAnywhere)
	double CombatRadius = 500.f;

	UPROPERTY(EditAnywhere, Category="Combat")
	float AttackRange = 1000.f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float StopChaseRange = 1500.f;

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
	FTimerHandle AttackTimer;

	void PatrolTimerFinished();

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float WaitMin = 5.f;

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float WaitMax = 10.f;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float PatrolSpeed = 100.f;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float ChaseSpeed = 400.f;
};
