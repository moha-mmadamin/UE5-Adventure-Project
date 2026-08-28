#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "EnemyTypes.h"
#include "CharacterTypes.h"
#include "Components/StaticMeshComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "Enemy.generated.h"

class AAIController;
class UStaticMeshComponent;
class UPerceptionComponent;
class UAIComponent;
class UAIPerceptionComponent;
class UAISenseConfig_Sight;

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

private:

    /*
    Combat
    */

    void StartAiming();
    void StopAiming();
    void BeginFiring();
    void TryFire();
    void TryReload();

    void Die();

    void DeactivatePerception();

    /*
    Components
    */

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    UPerceptionComponent* PerceptionComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    UAIComponent* AIComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    UAIPerceptionComponent* AIPerceptionComponent;

    /*
    Controller
    */

    UPROPERTY()
    AAIController* EnemyController;

    /*
    Equipment
    */

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment", meta = (AllowPrivateAccess = "true"))
    UStaticMeshComponent* HolsterMesh;

    /*
    Combat Settings
    */

    UPROPERTY(EditAnywhere, Category = "Combat")
    float AttackRange = 1000.f;

    UPROPERTY(EditAnywhere, Category = "Combat")
    float AimDelay = 0.5f;

    /*
    Combat Timers
    */

    FTimerHandle AimTimer;
    FTimerHandle AttackTimer;

public:
    FORCEINLINE UAIComponent* GetAIComponent() const { return AIComponent; }
    FORCEINLINE UPerceptionComponent* GetPerceptionComponent() const { return PerceptionComponent; }
    FORCEINLINE UAIPerceptionComponent* GetAIPerceptionComponent() const { return AIPerceptionComponent; }
};