#include "AI/Services/BTS_UpdateCombatRange.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "GameFramework/Actor.h"

UBTS_UpdateCombatRange::UBTS_UpdateCombatRange()
{
    NodeName = TEXT("Update Combat Range");

    Interval = 0.1f;
    RandomDeviation = 0.0f;
}
void UBTS_UpdateCombatRange::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

    AAIController* AIController = OwnerComp.GetAIOwner();
    if(!AIController) return;

    APawn* Enemy = AIController->GetPawn();
    if(!Enemy) return;

    UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
    if(!BB) return;

    AActor* TargetActor = Cast<AActor>(BB->GetValueAsObject(TEXT("TargetActor")));

    if (!TargetActor)
    {
        BB->SetValueAsBool(TEXT("IsInCombatRange"), false);
        return;
    }

    const float Distance = FVector::Dist(Enemy->GetActorLocation(), TargetActor->GetActorLocation());

    const bool bInRange = Distance <= CombatRange;

    BB->SetValueAsBool(TEXT("IsInCombatRange"), bInRange);
}
