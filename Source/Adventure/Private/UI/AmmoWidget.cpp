#include "UI/AmmoWidget.h"
#include "Weapons/BaseWeapon.h"
#include "Components/TextBlock.h"

void UAmmoWidget::BindWeapon(ABaseWeapon* Weapon)
{
    if(!Weapon) return;
    if (CurrentWeapon)
    {
        CurrentWeapon->OnAmmoChanged.RemoveDynamic(this, &UAmmoWidget::UpdateAmmo);
    }
    CurrentWeapon = Weapon;
    Weapon->OnAmmoChanged.AddDynamic(this, &UAmmoWidget::UpdateAmmo);
    UpdateAmmo(CurrentWeapon->GetCurrentAmmo(), CurrentWeapon->GetReserveAmmo());
}

void UAmmoWidget::UpdateAmmo(int32 CurrentAmmoValue, int32 ReserveAmmoValue)
{
    if (CurrentAmmo)
    {
        CurrentAmmo->SetText(FText::AsNumber(CurrentAmmoValue));

        FLinearColor AmmoColor;

        if (CurrentAmmoValue == CurrentWeapon->GetMagazineCapacity())
        {
            AmmoColor = FLinearColor::Green;
        }
        else if (CurrentAmmoValue == 0)
        {
            AmmoColor = FLinearColor::Red;
        }
        else
        {
            AmmoColor = FLinearColor::White;
        }

        CurrentAmmo->SetColorAndOpacity(FSlateColor(AmmoColor));
    }

    if (ReserveAmmo)
    {
        ReserveAmmo->SetText(FText::AsNumber(ReserveAmmoValue));
    }
}
