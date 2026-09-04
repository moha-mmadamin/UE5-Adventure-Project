#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EnemyHealthBar.generated.h"

class UProgressBarWidget;

UCLASS()
class ADVENTURE_API UEnemyHealthBar : public UUserWidget
{
	GENERATED_BODY()
	
public:
    UFUNCTION(BlueprintCallable)
    void SetHealthPercentage(float Percentage);

protected:
    UPROPERTY(meta = (BindWidget))
    UProgressBarWidget* ProgressBarWidget;

private:

};
