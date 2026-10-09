#include "AI/BTS_CheckCombatDistance.h"

#include "AIController.h"
#include "AI/EnemyAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Characters/Enemy.h"
#include "Characters/EnemyTypes.h"

UBTS_CheckCombatDistance::UBTS_CheckCombatDistance()
{
    NodeName = TEXT("Check Combat Distance");

    bNotifyTick = true;

    Interval = 0.2f;
    RandomDeviation = 0.05f;
}
void UBTS_CheckCombatDistance::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

    AAIController* Controller = OwnerComp.GetAIOwner();
    if(!Controller) return;

    AEnemy* Enemy = Cast<AEnemy>(Controller->GetPawn());
    if(!Enemy) return;

    UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
    if(!BB) return;

    AActor* Target = Cast<AActor>(BB->GetValueAsObject(TEXT("TargetActor")));
    if(!Target) return;

    const float Distance = FVector::Dist(Enemy->GetActorLocation(), Target->GetActorLocation());

    AEnemyAIController* EnemyController = Cast<AEnemyAIController>(Controller);
    if(!EnemyController) return;

}
