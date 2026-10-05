#include "AI/BTT_LookAround.h"
#include "AI/EnemyAIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"

UBTT_LookAround::UBTT_LookAround()
{
    NodeName = TEXT("Look Around");
    bNotifyTick = true;
}

EBTNodeResult::Type UBTT_LookAround::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AEnemyAIController* EnemyController = Cast<AEnemyAIController>(OwnerComp.GetAIOwner());

    if (!EnemyController)
    {
        return EBTNodeResult::Failed;
    }

    EnemyController->LookAround();

    return EBTNodeResult::InProgress;
}

void UBTT_LookAround::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    AEnemyAIController* EnemyController = Cast<AEnemyAIController>(OwnerComp.GetAIOwner());

    if (!EnemyController)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        return;
    }

    if (!EnemyController->IsLookingAround())
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
    }
}
