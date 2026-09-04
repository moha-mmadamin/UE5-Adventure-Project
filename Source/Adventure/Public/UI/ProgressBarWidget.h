#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ProgressBarWidget.generated.h"

class UProgressBar;
class UMaterialInstanceDynamic;

UCLASS()
class ADVENTURE_API UProgressBarWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
    UFUNCTION(BlueprintImplementableEvent)
    void SetPercentage(float Percentage);


protected:
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UProgressBar* HealthBar;

};
