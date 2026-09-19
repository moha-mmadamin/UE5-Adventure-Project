#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Perception/AIPerceptionTypes.h"
#include "PerceptionComponent.generated.h"

// Forward Declarations
class AEnemy;
class UAIComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ADVENTURE_API UPerceptionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UPerceptionComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


protected:
	virtual void BeginPlay() override;

private:	

	/*
	AI Perception
	*/

	void HandleSight(AActor* DetectedActor);
	void HandleLostSight(AActor* DetectedActor);
	void HandleHearing(const FVector& Location);
	void HandleDamage(AActor* DamageCauser);

	UFUNCTION()
	void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

	/*
	References
	*/

    UPROPERTY()
    AEnemy* OwnerEnemy;

    UPROPERTY()
    UAIComponent* AIComponent;

    UPROPERTY(VisibleAnywhere)
    UAIPerceptionComponent* AIPerceptionComponent;
		
public:

	/*
	Perception Events
	*/

	DECLARE_MULTICAST_DELEGATE_OneParam(FOnEnemySightDetected, AActor*);
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnEnemySightLost, AActor*);
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnEnemyNoiseHeard, const FVector&);
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnEnemyDamaged, AActor*);

    FOnEnemySightDetected OnSightDetected;
    FOnEnemySightLost OnSightLost;
    FOnEnemyNoiseHeard OnNoiseHeard;
    FOnEnemyDamaged OnDamaged;
};
