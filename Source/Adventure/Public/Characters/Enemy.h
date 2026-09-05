#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "EnemyTypes.h"
#include "CharacterTypes.h"
#include "UI/HealthBarWidget.h"
#include "Enemy.generated.h"

class AAIController;
class UStaticMeshComponent;
class UPerceptionComponent;
class UAIComponent;
class UAIPerceptionComponent;
class UAISenseConfig_Sight;
class UHealthComponent;
class UWidgetComponent;
class UEnemyHealthBar;

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
    UFUNCTION()
    virtual void Die() override;

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
    Health
    */

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health", meta = (AllowPrivateAccess = "true"))
    UHealthComponent* HealthComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health", meta = (AllowPrivateAccess = "true"))
    UWidgetComponent* HealthWidget;

    UPROPERTY()
    UEnemyHealthBar* EnemyHealthBar;

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