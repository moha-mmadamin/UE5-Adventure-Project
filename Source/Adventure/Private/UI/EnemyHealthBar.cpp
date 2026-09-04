#include "UI/EnemyHealthBar.h"
#include "UI/ProgressBarWidget.h"

void UEnemyHealthBar::SetHealthPercentage(float Percentage)
{
    if(ProgressBarWidget)
    {
        ProgressBarWidget->SetPercentage(Percentage);
    }
}