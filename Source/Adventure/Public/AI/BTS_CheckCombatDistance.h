#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTS_CheckCombatDistance.generated.h"

UCLASS()
class ADVENTURE_API UBTS_CheckCombatDistance : public UBTService
{
	GENERATED_BODY()

public:

    UBTS_CheckCombatDistance();

protected:

    virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:

    UPROPERTY(EditAnywhere, Category = "Combat")
    float CombatRange = 250.f;
	
};
