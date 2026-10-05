#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_LookAround.generated.h"

UCLASS()
class ADVENTURE_API UBTT_LookAround : public UBTTaskNode
{
	GENERATED_BODY()

public:
    UBTT_LookAround();

protected:
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
    virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

};
