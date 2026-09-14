#include "UI/AmmoWidget.h"
#include "Weapons/BaseWeapon.h"
#include "Components/TextBlock.h"

void UAmmoWidget::BindWeapon(ABaseWeapon* Weapon)
{
    if(!Weapon) return;
    CurrentWeapon = Weapon;
    Weapon->OnAmmoChanged.AddDynamic(this, &UAmmoWidget::UpdateAmmo);
}

void UAmmoWidget::UpdateAmmo(int CurrentAmmo, int MaxAmmo)
{
	FString Text = FString::Printf(TEXT("%d/%d"), CurrentAmmo, MaxAmmo);

    AmmoText->SetText(FText::FromString(Text));
}
