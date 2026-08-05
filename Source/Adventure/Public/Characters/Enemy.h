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
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	EEnemyState EnemyState = EEnemyState::EES_Idle;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	EEnemyDetectionType EnemyDetectionType = EEnemyDetectionType::EDT_None;

};
