#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyTypes.h"
#include "Enemy.generated.h"

UCLASS()
class ADVENTURE_API AEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	AEnemy();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category="AI")
	void HandleSight(AActor* DetectedActor);

	UFUNCTION(BlueprintCallable, Category = "AI")
	void HandleHearing(const FVector& Location);

	UFUNCTION(BlueprintCallable, Category="AI")
	void HandleDamage(AActor* DamageCauser);

private:
	void UpdateEnemyState();


	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	EEnemyState EnemyState = EEnemyState::EES_Idle;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	EEnemyDetectionType EnemyDetectionType = EEnemyDetectionType::EDT_None;

	UPROPERTY()
	AActor* CurrentTarget = nullptr;

	UPROPERTY()
	FVector LastKnownLocation = FVector::ZeroVector;
};
