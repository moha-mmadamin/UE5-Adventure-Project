#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/HealthComponent.h"
#include "HealthBarWidget.generated.h"

class UProgressBar;

UCLASS()
class ADVENTURE_API UHealthBarWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	void SetHealthComponent(UHealthComponent* Component);

	UFUNCTION()
	void UpdateHealthBar(float HealthPercent);

protected:
	UPROPERTY(meta = (BindWidget))
	UProgressBar* HealthBar;

	UPROPERTY()
	UHealthComponent* HealthComponent;
	
};
