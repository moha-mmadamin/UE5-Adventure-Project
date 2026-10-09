#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTS_UpdateCombatRange.generated.h"


UCLASS()
class ADVENTURE_API UBTS_UpdateCombatRange : public UBTService
{
	GENERATED_BODY()

public:

    UBTS_UpdateCombatRange();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float CombatRange = 2000.f;

protected:

    virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

};
