#include "UI/HealthBarWidget.h"
#include "Components/ProgressBar.h"


void UHealthBarWidget::UpdateHealthBar(float HealthPercent)
{
    if (HealthBar)
    {
        HealthBar->SetPercent(HealthPercent);
    }
}

void UHealthBarWidget::SetHealthComponent(UHealthComponent* Component)
{
    HealthComponent = Component;
    if(HealthComponent)
    {
        HealthComponent->OnHealthChanged.AddDynamic(this, &UHealthBarWidget::UpdateHealthBar);
        UpdateHealthBar(HealthComponent->GetHealthPercent());
    }
}