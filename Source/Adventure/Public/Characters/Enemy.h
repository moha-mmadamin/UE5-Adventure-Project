#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyTypes.h"
#include "Perception/AIPerceptionTypes.h"
#include "Enemy.generated.h"

class UAIPerceptionComponent;
class AAIController;

UCLASS()
class ADVENTURE_API AEnemy : public ACharacter
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

	UFUNCTION(BlueprintCallable, Category="AI")
	void HandleSight(AActor* DetectedActor);

	UFUNCTION(BlueprintCallable, Category = "AI")
	void HandleHearing(const FVector& Location);

	UFUNCTION(BlueprintCallable, Category="AI")
	void HandleDamage(AActor* DamageCauser);

private:
	void UpdateEnemyState();
	void CheckPatrolTarget();

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

	UPROPERTY(EditAnywhere)
	double CombatRadius = 500.f;

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
