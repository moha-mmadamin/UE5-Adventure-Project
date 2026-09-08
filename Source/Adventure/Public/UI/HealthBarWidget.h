#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Health/HealthComponent.h"
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

	UFUNCTION()
	void UpdateArmorBar(float ArmorPercent);

protected:
	UPROPERTY(meta = (BindWidget))
	UProgressBar* HealthBar;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* ArmorBar;

	UPROPERTY()
	UHealthComponent* HealthComponent;
	
};