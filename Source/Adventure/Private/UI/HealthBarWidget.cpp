#include "UI/HealthBarWidget.h"
#include "Components/ProgressBar.h"


void UHealthBarWidget::UpdateHealthBar(float HealthPercent)
{
    if(!HealthBar) return;

    HealthBar->SetPercent(HealthPercent);

    FLinearColor HealthColor;

    if (HealthPercent > 0.7f)
    {
        HealthColor = FLinearColor(0.00303f, 1.0f, 0.0f, 1.0f);
    }
    else if (HealthPercent > 0.5f)
    {
        HealthColor = FLinearColor(1.0f, 0.901f, 0.0f, 1.0f);
    }
    else if (HealthPercent > 0.25f)
    {
        HealthColor = FLinearColor(1.0f, 0.328f, 0.0f, 1.0f);
    }
    else
    {
        HealthColor = FLinearColor(1.0f, 0.0f, 0.0f, 1.0f);
    }
    HealthBar->SetFillColorAndOpacity(HealthColor);
}
void UHealthBarWidget::UpdateArmorBar(float ArmorPercent)
{
    if(ArmorBar)
    {
        ArmorBar->SetPercent(ArmorPercent);
    }
}
void UHealthBarWidget::SetHealthComponent(UHealthComponent* Component)
{
    HealthComponent = Component;

    if(!HealthComponent) return;
    HealthComponent->OnHealthChanged.AddDynamic(this, &UHealthBarWidget::UpdateHealthBar);
    HealthComponent->OnArmorChanged.AddDynamic(this, &UHealthBarWidget::UpdateArmorBar);

    UpdateHealthBar(HealthComponent->GetHealthPercent());
    UpdateArmorBar(HealthComponent->GetArmorPercent());
}